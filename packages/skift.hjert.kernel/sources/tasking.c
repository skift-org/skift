/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/assert.h>
#include <skift/cstring.h>
#include <skift/atomic.h>
#include <skift/elf.h>
#include <skift/math.h>

#include "kernel/cpu/irq.h"
#include "kernel/tasking.h"

/* -------------------------------------------------------------------------- */
/*   TASKING                                                                  */
/* -------------------------------------------------------------------------- */

static uint ticks = 0;
static thread_t *running = NULL;
process_t *kernel_process;
THREAD kernel_thread;

void tasking_setup()
{
    running = NULL;

    process_setup();
    thread_setup();

    kernel_process = process("kernel", false);
    kernel_thread = thread_create(kernel_process, NULL, NULL, 0);
    thread_create(kernel_process, garbage_colector, NULL, false);

    sheduler_setup(thread_getbyid(kernel_thread), kernel_process);
}

/* -------------------------------------------------------------------------- */
/*   THREADS                                                                  */
/* -------------------------------------------------------------------------- */

static int TID = 1;
static list_t *threads;
static list_t *threads_bystates[THREADSTATE_COUNT];

void thread_setup(void)
{
    threads = list();

    if (threads == NULL)
    {
        PANIC("Failed to allocate thread list!");
    }

    for (int i = 0; i < THREADSTATE_COUNT; i++)
    {
        threads_bystates[i] = list();

        if (threads_bystates[i] == NULL)
        {
            PANIC("Failled to allocate threadbystate list!");
        }
    }
}

thread_t *thread()
{
    thread_t *thread = MALLOC(thread_t);

    if (thread == NULL)
    {
        PANIC("Failed to allocated a new thread.");
    }

    memset(thread, 0, sizeof(thread_t));

    thread->id = TID++;
    thread->state = THREADSTATE_NONE;

    list_pushback(threads, thread);

    return thread;
}

void thread_delete(thread_t *thread)
{
    if (thread->state != THREADSTATE_NONE)
        thread_setstate(thread, THREADSTATE_NONE);

    list_remove(threads, thread);

    if (thread->process != NULL)
        list_remove(thread->process->threads, thread);

    // Now no one should still have a ptr to us we can die in peace.
    free(thread);
}

list_t *thread_bystate(thread_state_t state)
{
    return threads_bystates[state];
}

thread_t *thread_getbyid(int id)
{
    FOREACH(i, threads)
    {
        thread_t *thread = i->value;

        if (thread->id == id)
            return thread;
    }

    return NULL;
}

int thread_count(void)
{
    atomic_begin();
    int result = list_count(threads);
    atomic_end();

    return result;
}

THREAD thread_create_mainthread(process_t *parent_process, thread_entry_t entry, const char **argv)
{
    log(LOG_DEBUG, "Creating process %d main thread with eip@%08x.", parent_process->id, entry);

    atomic_begin();

    thread_t *t = thread();

    thread_setentry(t, entry, true);
    thread_attach_to_process(t, parent_process);

    uint argv_list[MAX_PROCESS_ARGV] = {0};

    int argc;
    for (argc = 0; argv[argc] && argc < MAX_PROCESS_ARGV; argc++)
    {
        argv_list[argc] = thread_stack_push(t, argv[argc], strlen(argv[argc]) + 1);
    }

    uint argv_list_ref = thread_stack_push(t, &argv_list, sizeof(argv_list));

    thread_stack_push(t, &argv_list_ref, sizeof(argv_list_ref));
    thread_stack_push(t, &argc, sizeof(argc));

    thread_setready(t);

    atomic_end();

    return t->id;
}

THREAD thread_create(process_t *parent_process, thread_entry_t entry, void *arg, bool user)
{
    ASSERT_ATOMIC;

    thread_t *t = thread();

    thread_setentry(t, entry, user);
    thread_attach_to_process(t, parent_process);
    thread_stack_push(t, &arg, sizeof(arg));
    thread_setready(t);

    return t->id;
}

/* --- Threads methodes ----------------------------------------------------- */

bool shortest_sleep_first(void *left, void *right)
{
    return ((thread_t *)left)->wait.time.wakeuptick < ((thread_t *)right)->wait.time.wakeuptick;
}

void thread_setstate(thread_t *thread, thread_state_t state)
{
    if (thread->state == state)
        return;

    // Remove the thread from its old groupe.
    if (thread->state != THREADSTATE_NONE)
    {
        list_remove(threads_bystates[thread->state], thread);
    }

    // Update the thread state
    thread->state = state;

    // Add the thread to the groupe
    if (thread->state != THREADSTATE_NONE)
    {
        if (thread->state == THREADSTATE_WAIT_TIME)
        {
            list_insert_sorted(threads_bystates[THREADSTATE_WAIT_TIME], thread, shortest_sleep_first);
        }
        else
        {
            list_push(threads_bystates[thread->state], thread);
        }
    }
}

void thread_setentry(thread_t *t, thread_entry_t entry, bool user)
{
    t->entry = entry;
    t->user = user;
    // setup the stack
    memset(t->stack, 0, MAX_THREAD_STACKSIZE);
    t->sp = (reg32_t)(&t->stack[0] + MAX_THREAD_STACKSIZE - 1);
}

uint thread_stack_push(thread_t *t, const void *value, uint size)
{
    t->sp -= size;
    memcpy((void *)t->sp, value, size);

    return t->sp;
}

void thread_attach_to_process(thread_t *t, process_t *p)
{
    if (t->process == NULL)
    {
        list_pushback(p->threads, t);
        t->process = p;
    }
    else
    {
        PANIC("Trying to attaching thread %d of process %d to process %d.", t->id, t->process->id, p->id);
    }
}

void thread_setready(thread_t *t)
{
    processor_context_t ctx;

    ctx.eflags = 0x202;
    ctx.eip = (reg32_t)t->entry;
    ctx.ebp = ((reg32_t)t->stack + MAX_THREAD_STACKSIZE);

    // TODO: userspace thread
    ctx.cs = 0x08;
    ctx.ds = 0x10;
    ctx.es = 0x10;
    ctx.fs = 0x10;
    ctx.gs = 0x10;

    thread_stack_push(t, &ctx, sizeof(ctx));

    thread_setstate(t, THREADSTATE_RUNNING);
}

/* --- Thread wait state ---------------------------------------------------- */

void thread_sleep(int time)
{
    ATOMIC({
        running->wait.time.wakeuptick = ticks + time;
        thread_setstate(running, THREADSTATE_WAIT_TIME);
    });

    sheduler_yield();
}

void thread_wakeup(THREAD t)
{
    atomic_begin();

    thread_t *thread = thread_getbyid(t);

    if (thread != NULL && thread->state == THREADSTATE_WAIT_TIME)
    {
        thread_setstate(running, THREADSTATE_RUNNING);
    }

    atomic_end();
}

bool thread_wait_thread(THREAD t, int *exitvalue)
{
    atomic_begin();

    thread_t *thread = thread_getbyid(t);

    if (thread != NULL)
    {
        if (thread->state == THREADSTATE_CANCELED)
        {
            if (exitvalue != NULL)
            {
                *exitvalue = thread->exitvalue;
            }

            atomic_end();
        }
        else
        {
            running->wait.thread.thread_handle = t;
            thread_setstate(running, THREADSTATE_WAIT_THREAD);

            atomic_end();
            sheduler_yield();

            if (exitvalue != NULL)
            {
                *exitvalue = running->wait.thread.exitvalue;
            }
        }

        return true;
    }
    else
    {
        atomic_end();
        return false;
    }
}

bool thread_wait_process(PROCESS p, int *exitvalue)
{
    atomic_begin();

    process_t *process = process_getbyid(p);

    if (process != NULL)
    {
        if (process->state == PROCESS_CANCELED || process->state == PROCESS_CANCELING)
        {
            if (exitvalue != NULL)
            {
                *exitvalue = process->exitvalue;
            }

            atomic_end();
        }
        else
        {
            running->wait.process.process_handle = p;
            thread_setstate(running, THREADSTATE_WAIT_PROCESS);

            atomic_end();
            sheduler_yield();

            if (exitvalue != NULL)
            {
                *exitvalue = running->wait.process.exitvalue;
            }
        }

        return true;
    }
    else
    {
        atomic_end();
        return false;
    }
}

/* --- Thread stopping and canceling ---------------------------------------- */

bool thread_cancel(THREAD t, int exitvalue)
{
    atomic_begin();

    thread_t *thread = thread_getbyid(t);

    if (thread != NULL && thread->process == running->process)
    {
        // Set the new thread state
        thread->exitvalue = exitvalue;
        thread_setstate(running, THREADSTATE_CANCELED);

        log(LOG_DEBUG, "Thread(%d) got canceled.", t);

        // Wake up waiting threads
        FOREACH(i, thread->process->threads)
        {
            thread_t *waitthread = i->value;

            if (waitthread->state == THREADSTATE_WAIT_THREAD &&
                waitthread->wait.thread.thread_handle == t)
            {
                waitthread->wait.thread.exitvalue = exitvalue;
                thread_setstate(waitthread, THREADSTATE_RUNNING);

                log(LOG_DEBUG, "Thread %d finish waiting thread %d.", waitthread->id, thread->id);
            }
        }

        atomic_end();
        return true;
    }
    else
    {
        atomic_end();
        return false;
    }
}

void thread_exit(int exitvalue)
{
    thread_cancel(running->id, exitvalue);

    sheduler_yield();

    PANIC("sheduler_yield return but the thread is canceled!");
}

/* --- Thread dump ---------------------------------------------------------- */

static char *THREAD_STATES[] =
    {
        "RUNNING",
        "SLEEP",
        "WAIT(thread)",
        "WAIT(process)",
        "WAIT(message)",
        "CANCELED",
};

void thread_dump(thread_t *t)
{
    atomic_begin();
    printf("\n\t . Thread %d", t->id);
    printf("\n\t   State: %s", THREAD_STATES[t->state]);
    printf("\n\t   Process: %d %s", t->id, t->process->name);
    printf("\n\t   User memory: ");
    memory_layout_dump(t->process->pdir, true);
    printf("\n");
    atomic_end();
}

void thread_panic_dump(void)
{
    atomic_begin();

    printf("\n");
    printf("\n\tRunning thread %d", sheduler_running_thread_id());
    printf("\n");
    printf("\n\tThreads:");

    FOREACH(i, threads)
    {
        thread_t *t = i->value;
        thread_dump(t);
    }

    atomic_end();
}

/* -------------------------------------------------------------------------- */
/*   PROCESSES                                                                */
/* -------------------------------------------------------------------------- */

static int PID = 1;
static list_t *processes;

void process_setup(void)
{
    processes = list();
}

process_t *process(const char *name, bool user)
{
    ASSERT_ATOMIC;

    process_t *process = MALLOC(process_t);

    if (process == NULL)
    {
        PANIC("Failed to allocated a new process.");
    }

    process->id = PID++;

    strncpy(process->name, name, MAX_PROCESS_NAMESIZE);
    process->user = user;
    process->threads = list();
    process->inbox = list();

    // Setup fildes
    lock_init(process->fds_lock);
    for (int i = 0; i < MAX_PROCESS_OPENED_FILES; i++)
    {
        process_filedescriptor_t *fd = &process->fds[i];
        fd->stream = NULL;
        fd->free = true;
        lock_init(fd->lock);
    }

    // Setup cwd.
    process_t *parent = sheduler_running_process();

    lock_init(process->cwd_lock);

    if (parent != NULL)
    {
        parent->cwd_node->refcount++;
        process->cwd_node = parent->cwd_node;
        process->cwd_path = path_dup(parent->cwd_path);
    }
    else
    {
        path_t *p = path("/");
        process->cwd_node = filesystem_acquire(NULL, p, false);
        process->cwd_path = p;
    }

    // Setup virtual memnory
    if (user)
    {
        process->pdir = memory_alloc_pdir();
    }
    else
    {
        process->pdir = memory_kpdir();
    }

    log(LOG_FINE, "Process '%s' with ID=%d Created.", process->name, process->id);

    list_pushback(processes, process);
    return process;
}

void process_delete(process_t *this)
{
    if (this->pdir != memory_kpdir())
    {
        memory_free_pdir(this->pdir);
    }

    assert(!list_any(this->processes));
    list_delete(this->processes, LIST_KEEP_VALUES);

    assert(!list_any(this->threads));
    list_delete(this->threads, LIST_KEEP_VALUES);

    assert(!list_any(this->inbox));
    list_delete(this->inbox, LIST_KEEP_VALUES);

    list_remove(processes, this);

    free(this);
}

process_t *process_getbyid(PROCESS pid)
{
    FOREACH(i, processes)
    {
        process_t *p = (process_t *)i->value;

        if (p->id == pid)
            return p;
    }

    return NULL;
}

int process_count(void)
{
    int result;

    atomic_begin();

    result = list_count(processes);

    atomic_end();

    return result;
}

/* --- Process exit and canceling ------------------------------------------- */

bool process_cancel(process_t *self, int exitvalue)
{
    atomic_begin();

    if (self != kernel_process)
    {
        // Set our new process state
        self->state = PROCESS_CANCELED;
        self->exitvalue = exitvalue;

        log(LOG_DEBUG, "Process '%s' ID=%d canceled!", self->name, self->id);

        // Wake up waiting threads
        FOREACH(i, thread_bystate(THREADSTATE_WAIT_PROCESS))
        {
            thread_t *thread = i->value;

            if (thread->wait.process.process_handle == self->id)
            {
                thread->wait.process.exitvalue = exitvalue;
                thread_setstate(thread, THREADSTATE_RUNNING);
                log(LOG_DEBUG, "Thread %d finish waiting process %d.", thread->id, self->id);
            }
        }

        // Cancel childs threads.
        FOREACH(i, self->threads)
        {
            thread_t *thread = (thread_t *)i->value;
            thread_cancel(thread->id, 0);
        }

        atomic_end();
        return true;
    }
    else
    {
        process_t *process = sheduler_running_process();
        log(LOG_WARNING, "Process '%s' ID=%d tried to commit murder on the kernel!", process->name, process->id);

        atomic_end();
        return false;
    }
}

void process_exit(int exitvalue)
{
    process_t *self = sheduler_running_process();

    if (self != kernel_process)
    {
        process_cancel(self, exitvalue);

        sheduler_yield();

        PANIC("sheduler_yield return but the process is stopped!");
    }
    else
    {
        PANIC("Kernel try to commit suicide!");
    }
}

/* --- Process elf file loading --------------------------------------------- */

void load_elfseg(process_t *process, iostream_t *s, elf_program_t *program)
{
    log(LOG_DEBUG, "Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", program->offset, program->filesz, program->vaddr, program->memsz);

    if (program->vaddr >= 0x100000)
    {
        atomic_begin();

        // To avoid pagefault we need to switch page directorie.
        page_directorie_t *pdir = running->process->pdir;

        paging_load_directorie(process->pdir);
        process_memory_map(process, program->vaddr, PAGE_ALIGN(program->memsz) / PAGE_SIZE + PAGE_SIZE);
        memset((void *)program->vaddr, 0, program->memsz);

        iostream_seek(s, program->offset, IOSTREAM_WHENCE_START);
        iostream_read(s, (void *)program->vaddr, program->filesz);

        paging_load_directorie(pdir);

        atomic_end();
    }
    else
    {
        log(LOG_WARNING, "Elf segment ignored, not in user memory!");
    }
}

PROCESS process_exec(const char *executable_path, const char **argv)
{
    // Check if the file existe and open the file.
    iostream_t *s = iostream_open(executable_path, IOSTREAM_READ);

    if (s == NULL)
    {
        log(LOG_WARNING, "'%s' file not found, exec failed!", executable_path);
        return -1;
    }

    // Check if the file isn't a directory.
    iostream_stat_t stat;
    iostream_fstat(s, &stat);

    if (stat.type != IOSTREAM_TYPE_REGULAR)
    {
        log(LOG_WARNING, "'%s' is not a file, exec failed!", executable_path);
        iostream_close(s);
        return -1;
    }

    // Decode the elf file header.
    elf_header_t elf_header = {0};
    iostream_seek(s, 0, IOSTREAM_WHENCE_START);
    iostream_read(s, &elf_header, sizeof(elf_header_t));

    if (!elf_valid(&elf_header))
    {
        log(LOG_WARNING, "Invalid elf program!", executable_path);
        iostream_close(s);
        return -1;
    }

    // Create the process and load the executable.
    atomic_begin();

    process_t *new_process = process(executable_path, true);
    int new_process_id = new_process->id;

    elf_program_t program;

    for (int i = 0; i < elf_header.phnum; i++)
    {
        iostream_seek(s, elf_header.phoff + (elf_header.phentsize * i), IOSTREAM_WHENCE_START);
        iostream_read(s, &program, sizeof(elf_program_t));

        load_elfseg(new_process, s, &program);
    }

    log(LOG_DEBUG, "Executable loaded, creating main thread...");

    thread_create_mainthread(new_process, (thread_entry_t)elf_header.entry, argv);

    log(LOG_DEBUG, "Process created, back to caller..");

    atomic_end();

    iostream_close(s);
    return new_process_id;
}

/* --- Current working directory -------------------------------------------- */

path_t* process_cwd_resolve(process_t* this, const char* path_to_resolve)
{
    path_t *p = path(path_to_resolve);

    if (path_is_relative(p))
    {
        lock_acquire(this->cwd_lock);

        path_t *combined = path_combine(this->cwd_path, p);
        path_delete(p);
        p = combined;

        lock_release(this->cwd_lock);
    }

    path_normalize(p);

    return p;
}

bool process_set_cwd(process_t *this, const char *new_path)
{
    log(LOG_DEBUG, "Process %d set cwd to '%s'", this->id, new_path);
    
    path_t *work_path = process_cwd_resolve(this, new_path);
    
    lock_acquire(this->cwd_lock);

    fsnode_t *new_cwd = filesystem_acquire(NULL, work_path, false);

    if (new_cwd != NULL && fsnode_to_iostream_type(new_cwd->type) == IOSTREAM_TYPE_DIRECTORY)
    {
        // Cleanup the old path
        path_delete(this->cwd_path);
        filesystem_release(this->cwd_node);

        // Set the new path
        this->cwd_node = new_cwd;
        this->cwd_path = work_path;

        lock_release(this->cwd_lock);

        return true;
    }
    else
    {
        if (new_cwd != NULL)
        {
            filesystem_release(new_cwd);
        }

        path_delete(work_path);
        lock_release(this->cwd_lock);

        return false;
    }
}

void process_get_cwd(process_t *this, char *buffer, uint size)
{
    lock_acquire(this->cwd_lock);

    path_to_cstring(this->cwd_path, buffer, size);

    lock_release(this->cwd_lock);
}

/* File descriptor allocation and locking ----------------------------------- */

void process_filedescriptor_close_all(process_t *this)
{
    for (int i = 0; i < MAX_PROCESS_OPENED_FILES; i++)
    {
        process_close_file(this, i);
    }
}

int process_filedescriptor_alloc_and_acquire(process_t *this, stream_t *stream)
{
    lock_acquire(this->fds_lock);

    for (int i = 0; i < MAX_PROCESS_OPENED_FILES; i++)
    {
        process_filedescriptor_t *fd = &this->fds[i];

        if (fd->free)
        {
            fd->free = false;
            fd->stream = stream;
            lock_acquire(fd->lock);

            lock_release(this->fds_lock);

            log(LOG_DEBUG, "File descriptor %d allocated for process %d'%s'", i, this->id, this->name);

            return i;
        }
    }

    lock_release(this->fds_lock);
    log(LOG_WARNING, "We run out of file descriptor on process %d'%s'", this->id, this->name);

    return -1;
}

stream_t *process_filedescriptor_acquire(process_t *this, int fd_index)
{
    if (fd_index >= 0 && fd_index < MAX_PROCESS_OPENED_FILES)
    {
        process_filedescriptor_t *fd = &this->fds[fd_index];
        lock_acquire(fd->lock);

        if (!fd->free)
        {
            return fd->stream;
        }
    }

    log(LOG_WARNING, "Got a bad file descriptor %d from process %d'%s'", fd_index, this->id, this->name);

    return NULL;
}

int process_filedescriptor_release(process_t *this, int fd_index)
{
    if (fd_index >= 0 && fd_index < MAX_PROCESS_OPENED_FILES)
    {
        process_filedescriptor_t *fd = &this->fds[fd_index];

        lock_release(fd->lock);

        return 0;
    }

    log(LOG_WARNING, "Got a bad file descriptor %d from process %d'%s'", fd_index, this->id, this->name);

    return -1;
}

int process_filedescriptor_free_and_release(process_t *this, int fd_index)
{
    if (fd_index >= 0 && fd_index < MAX_PROCESS_OPENED_FILES)
    {
        process_filedescriptor_t *fd = &this->fds[fd_index];

        lock_release(fd->lock);

        fd->free = true;
        fd->stream = NULL;

        log(LOG_DEBUG, "File descriptor %d free for process %d'%s'", fd_index, this->id, this->name);

        return 0;
    }

    log(LOG_WARNING, "Got a bad file descriptor %d from process %d'%s'", fd_index, this->id, this->name);

    return -1;
}

/* --- Process file operations -------------------------------------------------- */

int process_open_file(process_t *this, const char *file_path, iostream_flag_t flags)
{
    path_t *p = process_cwd_resolve(this, file_path);

    stream_t *stream = filesystem_open(NULL, p, flags);

    path_delete(p);

    if (stream == NULL)
    {
        return -1;
    }

    int fd = process_filedescriptor_alloc_and_acquire(this, stream);

    if (fd != -1)
    {
        process_filedescriptor_release(this, fd);
    }
    else
    {
        filesystem_close(stream);
    }

    return fd;
}

int process_close_file(process_t *this, int fd)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -1;
    }

    filesystem_close(stream);

    process_filedescriptor_free_and_release(this, fd);

    return 0;
}

int process_read_file(process_t *this, int fd, void *buffer, uint size)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_read(stream, buffer, size);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_write_file(process_t *this, int fd, const void *buffer, uint size)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_write(stream, buffer, size);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_ioctl_file(process_t *this, int fd, int request, void *args)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_ioctl(stream, request, args);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_seek_file(process_t *this, int fd, int offset, iostream_whence_t whence)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_seek(stream, offset, whence);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_tell_file(process_t *this, int fd, iostream_whence_t whence)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_tell(stream, whence);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_fstat_file(process_t *this, int fd, iostream_stat_t *stat)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_fstat(stream, stat);

    process_filedescriptor_release(this, fd);

    return result;
}

/* --- Process Memory managment --------------------------------------------- */

int process_memory_map(process_t *p, uint addr, uint count)
{
    return memory_map(p->pdir, addr, count, 1);
}

int process_memory_unmap(process_t *p, uint addr, uint count)
{
    return memory_unmap(p->pdir, addr, count);
}

uint process_memory_alloc(process_t *p, uint count)
{
    uint addr = memory_alloc(p->pdir, count, 1);
    log(LOG_DEBUG, "Gived userspace %d memory block at 0x%08x", count, addr);
    return addr;
}

void process_memory_free(process_t *p, uint addr, uint count)
{
    log(LOG_DEBUG, "Userspace free'd %d memory block at 0x%08x", count, addr);
    return memory_free(p->pdir, addr, count, 1);
}

/* -------------------------------------------------------------------------- */
/*   MESSAGING                                                                */
/* -------------------------------------------------------------------------- */

static int MID = 1;
static list_t *channels;

/* --- Channel -------------------------------------------------------------- */

channel_t *channel(const char *name)
{
    channel_t *channel = MALLOC(channel_t);

    channel->subscribers = list();
    strncpy(channel->name, name, CHANNAME_SIZE);

    return channel;
}

void channel_delete(channel_t *channel)
{
    list_delete(channel->subscribers, LIST_KEEP_VALUES);
    free(channel);
}

channel_t *channel_get(const char *channel_name)
{
    FOREACH(i, channels)
    {
        channel_t *c = (channel_t *)i->value;

        if (strcmp(channel_name, c->name) == 0)
            return c;
    }

    return NULL;
}

/* --- Message -------------------------------------------------------------- */

message_t *message(int id, const char *label, void *payload, uint size, uint flags)
{
    message_t *message = MALLOC(message_t);

    if (payload != NULL && size > 0)
    {
        message->size = min(MSGPAYLOAD_SIZE, size);
        message->payload = malloc(message->size);
        memcpy(message->payload, payload, size);
    }
    else
    {
        message->size = 0;
        message->payload = NULL;
    }

    message->id = id;
    message->flags = flags;
    strncpy(message->label, label, MSGLABEL_SIZE);

    return message;
}

void message_delete(message_t *msg)
{
    if (msg->payload)
        free(msg->payload);
    free(msg);
}

uint messaging_id()
{
    uint id;

    ATOMIC({
        id = MID++;
    });

    return id;
}

/* --- Messaging API -------------------------------------------------------- */

void messaging_setup(void)
{
    channels = list();
}

int messaging_send_internal(PROCESS from, PROCESS to, int id, const char *name, void *payload, uint size, uint flags)
{
    process_t *process = process_getbyid(to);

    if (process == NULL)
    {
        return 0;
    }

    if (process->inbox->count > 1024)
    {
        log(LOG_WARNING, "PROC=%d inbox is full!", to);
        return 0;
    }

    message_t *msg = message(id, name, payload, size, flags);

    msg->from = from;
    msg->to = to;

    list_pushback(process->inbox, (void *)msg);

    FOREACH(t, process->threads)
    {
        thread_t *thread = t->value;

        if (thread->state == THREADSTATE_WAIT_MESSAGE)
        {
            messaging_receive_internal(thread);
            thread_setstate(thread, THREADSTATE_RUNNING);
            break;
        }
    }

    return id;
}

int messaging_send(PROCESS to, const char *name, void *payload, uint size, uint flags)
{
    int id = 0;

    ATOMIC({
        id = messaging_send_internal(sheduler_running_process_id(), to, messaging_id(), name, payload, size, flags);
    });

    return id;
}

int messaging_broadcast(const char *channel_name, const char *name, void *payload, uint size, uint flags)
{
    int id = 0;

    atomic_begin();

    channel_t *c = channel_get(channel_name);

    if (c != NULL)
    {
        id = messaging_id();

        FOREACH(p, c->subscribers)
        {
            messaging_send_internal(sheduler_running_process_id(), ((process_t *)p->value)->id, id, name, payload, size, flags);
        }
    }

    atomic_end();

    return id;
}

message_t *messaging_receive_internal(thread_t *thread)
{
    message_t *msg = NULL;

    atomic_begin();

    if (thread->process->inbox->count > 0)
    {
        if (thread->wait.message.message != NULL)
        {
            message_delete(thread->wait.message.message);
        }

        list_pop(thread->process->inbox, (void **)&msg);
        thread->wait.message.message = msg;
    }

    atomic_end();
    return msg;
}

bool messaging_receive(message_t *msg, bool wait)
{
    message_t *incoming = messaging_receive_internal(sheduler_running_thread());

    if (incoming == NULL && wait)
    {
        atomic_begin();
        thread_setstate(sheduler_running_thread(), THREADSTATE_WAIT_MESSAGE);
        atomic_end();

        sheduler_yield(); // Wait until we get a message.

        incoming = sheduler_running_thread()->wait.message.message;
    }

    if (incoming != NULL)
    {
        memcpy(msg, incoming, sizeof(message_t));
        return true;
    }

    return false;
}

int messaging_payload(void *buffer, uint size)
{
    message_t *incoming = sheduler_running_thread()->wait.message.message;

    if (incoming != NULL && incoming->size > 0 && incoming->payload != NULL)
    {
        memcpy(buffer, incoming->payload, min(size, incoming->size));
        return 0;
    }

    return 1;
}

int messaging_subscribe(const char *channel_name)
{
    atomic_begin();

    channel_t *c = channel_get(channel_name);

    if (c == NULL)
    {
        c = channel(channel_name);
        list_pushback(channels, c);
    }

    list_pushback(c->subscribers, sheduler_running_thread()->process);

    atomic_end();

    return 0;
}

int messaging_unsubscribe(const char *channel_name)
{
    atomic_begin();

    channel_t *c = channel_get(channel_name);

    if (c != NULL)
    {
        list_remove(c->subscribers, sheduler_running_thread()->process);
    }

    atomic_end();

    return 0;
}

/* -------------------------------------------------------------------------- */
/*   GARBAGE COLECTOR                                                         */
/* -------------------------------------------------------------------------- */

void collect_and_free_thread(void)
{
    list_t *thread_to_free = list();

    // Search for thread with a canceled parent process.
    FOREACH(i, thread_bystate(THREADSTATE_CANCELED))
    {
        thread_t *thread = i->value;

        if (thread->process->state == PROCESS_CANCELED)
        {
            list_pushback(thread_to_free, thread);
        }
    }

    // Cleanup all of those dead threads.
    FOREACH(i, thread_to_free)
    {
        thread_t *thread = i->value;
        int thread_id = thread->id;

        log(LOG_DEBUG, "free'ing thread %d", thread_id);

        thread_delete(thread);

        log(LOG_DEBUG, "Thread %d free'd.", thread_id);
    }

    list_delete(thread_to_free, LIST_KEEP_VALUES);
}

void garbage_colector()
{
    while (true)
    {
        thread_sleep(100); // We don't do this really often.

        // log(LOG_DEBUG, "Garbage collect begin %d !", ticks);

        atomic_begin();
        collect_and_free_thread();
        atomic_end();

        // log(LOG_DEBUG, "Garbage collect end!");
    }
}

/* -------------------------------------------------------------------------- */
/*   SHEDULER                                                                 */
/* -------------------------------------------------------------------------- */

static bool sheduler_context_switch = false;

static thread_t idle;
void idle_code()
{
    while (1)
    {
        hlt();
    }
}

void timer_set_frequency(int hz)
{
    u32 divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    log(LOG_DEBUG, "Timer frequency is %dhz.", hz);
}

void sheduler_setup(thread_t *main_kernel_thread, process_t *kernel_process)
{
    running = main_kernel_thread;

    // Create the idle thread.
    memset(&idle, 0, sizeof(thread_t));
    idle.id = -1;
    thread_setentry(&idle, idle_code, false);
    thread_attach_to_process(&idle, kernel_process);
    thread_setready(&idle);

    timer_set_frequency(100);
    irq_register(0, (irq_handler_t)&shedule);
}

/* --- Sheduling ------------------------------------------------------------ */

void wakeup_sleeping_threads(void)
{
    if (!list_empty(thread_bystate(THREADSTATE_WAIT_TIME)))
    {
        thread_t *t;

        do
        {
            list_peek(thread_bystate(THREADSTATE_WAIT_TIME), (void **)&t);

            if (t->wait.time.wakeuptick <= ticks)
            {
                thread_setstate(t, THREADSTATE_RUNNING);
                // log(LOG_DEBUG, "Thread %d wake up!", t->id);
            }

        } while (t->stack == THREADSTATE_RUNNING);
    }
}

#include "kernel/dev/vga.h"

static const char *animation = "|/-\\|/-\\";

reg32_t shedule(reg32_t sp, processor_context_t *context)
{
    //log(LOG_DEBUG, "Current thread %d(%s) with eip@%08x.", running->id, running->process->name, context->eip);
    UNUSED(context);
    sheduler_context_switch = true;

    // Save the old context
    running->sp = sp;

    // Update the system ticks
    ticks++;
    wakeup_sleeping_threads();

    // Get the next thread
    if (!list_peek_and_pushback(thread_bystate(THREADSTATE_RUNNING), (void **)&running))
    {
        // Or the idle thread if there are no running threads.
        running = &idle;
    }

    vga_cell(VGA_SCREEN_WIDTH - 1, VGA_SCREEN_HEIGHT - 1, VGA_ENTRY(animation[(ticks / 100) % sizeof(animation)], VGACOLOR_WHITE, (running == &idle) ? VGACOLOR_BLACK : VGACOLOR_GREEN));

    // Restore the context
    // TODO: set_kernel_stack(...);
    paging_load_directorie(running->process->pdir);
    paging_invalidate_tlb();

    sheduler_context_switch = false;

    //log(LOG_DEBUG, "Now current thread is %d(%s)", running->id, running->process->name);

    return running->sp;
}

/* --- Sheduler info -------------------------------------------------------- */

uint sheduler_get_ticks(void)
{
    return ticks;
}

bool sheduler_is_context_switch(void)
{
    return sheduler_context_switch;
}

void sheduler_yield()
{
    asm("int $32");
}

/* --- Running thread info -------------------------------------------------- */

process_t *sheduler_running_process(void)
{
    if (running != NULL)
    {
        return running->process;
    }
    else
    {
        return NULL;
    }
}

int sheduler_running_process_id(void)
{
    if (running != NULL)
    {
        return running->process->id;
    }
    else
    {
        return -1;
    }
}

thread_t *sheduler_running_thread(void)
{
    return running;
}

int sheduler_running_thread_id(void)
{
    if (running != NULL)
    {
        return running->id;
    }
    else
    {
        return -1;
    }
}
