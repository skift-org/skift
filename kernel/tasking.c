/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/assert.h>
#include <skift/cstring.h>
#include <skift/atomic.h>
#include <skift/elf.h>
#include <skift/math.h>
#include <skift/error.h>

#include <hjert/cpu/irq.h>
#include <hjert/tasking.h>

/* -------------------------------------------------------------------------- */
/*   TASKING                                                                  */
/* -------------------------------------------------------------------------- */

static uint ticks = 0;
static task_t *running = NULL;

static task_t* kernel_task;
static task_t* garbage_task;
static task_t* idle_task;

void idle_code(){ HANG; }

void tasking_setup()
{
    running = NULL;

    task_setup();

    kernel_task = task_spawn(NULL, "kernel", NULL, NULL, 0);
    task_go(kernel_task);

    garbage_task = task_spawn(kernel_task, "finalizer", garbage_colector, NULL, false);
    task_go(garbage_task);

    idle_task = task_spawn(kernel_task, "idle", idle_code, NULL, false);
    task_go(idle_task);
    task_setstate(idle_task, TASK_STATE_HANG);

    sheduler_setup(kernel_task);
}

/* -------------------------------------------------------------------------- */
/*   TASKS                                                                    */
/* -------------------------------------------------------------------------- */

static int TID = 1;
static list_t *tasks;
static list_t *tasks_bystates[TASK_STATE_COUNT];

void task_setup(void)
{
    tasks = list();

    if (tasks == NULL)
    {
        PANIC("Failed to allocate task list!");
    }

    for (int i = 0; i < TASK_STATE_COUNT; i++)
    {
        tasks_bystates[i] = list();

        if (tasks_bystates[i] == NULL)
        {
            PANIC("Failled to allocate taskbystate list!");
        }
    }
}

task_t *task(task_t* parent, const char* name, bool user)
{
    ASSERT_ATOMIC;

    task_t *this = MALLOC(task_t);

    if (this == NULL)
    {
        PANIC("Failed to allocated a new task.");
    }

    memset(this, 0, sizeof(task_t));

    this->id = TID++;
    strlcpy(this->name, name, TASK_NAMESIZE);
    this->state = TASK_STATE_NONE;

    list_pushback(tasks, this);

    // Setup inbox
    lock_init(this->inbox_lock);
    this->inbox = list();

    // Setup current working directory.
    lock_init(this->cwd_lock);

    if (parent != NULL)
    {
        parent->cwd_node->refcount++;
        this->cwd_node = parent->cwd_node;
        this->cwd_path = path_dup(parent->cwd_path);
    }
    else
    {
        path_t *p = path("/");
        this->cwd_node = filesystem_acquire(NULL, p, false);
        this->cwd_path = p;
    }

    // Setup fildes
    lock_init(this->fds_lock);
    for (int i = 0; i < TASK_FILDES_COUNT; i++)
    {
        filedescriptor_t *fd = &this->fds[i];
        fd->stream = NULL;
        fd->free = true;
        lock_init(fd->lock);
    }

    // Setup memory space
    if (user)
    {
        this->pdir = memory_alloc_pdir();
    }
    else
    {
        this->pdir = memory_kpdir();
    }

    return this;
}

void task_delete(task_t *this)
{
    atomic_begin();
    if (this->state != TASK_STATE_NONE)
        task_setstate(this, TASK_STATE_NONE);

    list_remove(tasks, this);
    atomic_end();

    
    list_foreach(i, this->inbox)
    {
        message_delete(i->value);
    }
    list_delete(this->inbox, LIST_KEEP_VALUES);

    task_filedescriptor_close_all(this);

    lock_acquire(this->cwd_lock);
    path_delete(this->cwd_path);
    filesystem_release(this->cwd_node);

    if (this->pdir != memory_kpdir())
    {
        memory_free_pdir(this->pdir);
    }

    free(this);
}

list_t *task_bystate(task_state_t state)
{
    return tasks_bystates[state];
}

task_t *task_getbyid(int id)
{
    list_foreach(i, tasks)
    {
        task_t *task = i->value;

        if (task->id == id)
            return task;
    }

    return NULL;
}

int task_count(void)
{
    atomic_begin();
    int result = list_count(tasks);
    atomic_end();

    return result;
}

task_t* task_spawn(task_t* parent, const char* name, task_entry_t entry, void *arg, bool user)
{
    ASSERT_ATOMIC;

    task_t *t = task(parent, name, user);

    task_setentry(t, entry, user);
    task_stack_push(t, &arg, sizeof(arg));

    return t;
}

task_t* task_spawn_with_argv(task_t* parent, const char* name, task_entry_t entry, const char **argv, bool user)
{
    atomic_begin();

    task_t *t = task(parent, name, user);

    task_setentry(t, entry, true);

    uint argv_list[TASK_ARGV_COUNT] = {0};

    int argc;
    for (argc = 0; argv[argc] && argc < TASK_ARGV_COUNT; argc++)
    {
        argv_list[argc] = task_stack_push(t, argv[argc], strlen(argv[argc]) + 1);
    }

    uint argv_list_ref = task_stack_push(t, &argv_list, sizeof(argv_list));

    task_stack_push(t, &argv_list_ref, sizeof(argv_list_ref));
    task_stack_push(t, &argc, sizeof(argc));

    atomic_end();

    return t;
}

/* --- Tasks methodes ----------------------------------------------------- */

bool shortest_sleep_first(void *left, void *right)
{
    return ((task_t *)left)->wait.time.wakeuptick < ((task_t *)right)->wait.time.wakeuptick;
}

void task_setstate(task_t *task, task_state_t state)
{
    ASSERT_ATOMIC;

    if (task->state == state)
        return;

    // Remove the task from its old groupe.
    if (task->state != TASK_STATE_NONE)
    {
        list_remove(tasks_bystates[task->state], task);
    }

    // Update the task state
    task->state = state;

    // Add the task to the groupe
    if (task->state != TASK_STATE_NONE)
    {
        if (task->state == TASK_STATE_WAIT_TIME)
        {
            list_insert_sorted(tasks_bystates[TASK_STATE_WAIT_TIME], task, shortest_sleep_first);
        }
        else
        {
            list_push(tasks_bystates[task->state], task);
        }
    }
}

void task_setentry(task_t *t, task_entry_t entry, bool user)
{
    t->entry = entry;
    t->user = user;
    // setup the stack
    memset(t->stack, 0, TASK_STACKSIZE);
    t->sp = (reg32_t)(&t->stack[0] + TASK_STACKSIZE - 1);
}

uint task_stack_push(task_t *t, const void *value, uint size)
{
    t->sp -= size;
    memcpy((void *)t->sp, value, size);

    return t->sp;
}

void task_go(task_t *t)
{
    processor_context_t ctx;

    ctx.eflags = 0x202;
    ctx.eip = (reg32_t)t->entry;
    ctx.ebp = ((reg32_t)t->stack + TASK_STACKSIZE);

    // TODO: userspace task
    ctx.cs = 0x08;
    ctx.ds = 0x10;
    ctx.es = 0x10;
    ctx.fs = 0x10;
    ctx.gs = 0x10;

    task_stack_push(t, &ctx, sizeof(ctx));

    task_setstate(t, TASK_STATE_RUNNING);
}

/* --- Task wait state ---------------------------------------------------- */

void task_sleep(int time)
{
    ATOMIC({
        running->wait.time.wakeuptick = ticks + time;
        task_setstate(running, TASK_STATE_WAIT_TIME);
    });

    sheduler_yield();
}

int task_wakeup(task_t* task)
{
    ASSERT_ATOMIC;

    if (task != NULL && task->state == TASK_STATE_WAIT_TIME)
    {
        task_setstate(running, TASK_STATE_RUNNING);
        return 0;
    }

    return -1;
}

bool task_wait(int task_id, int *exitvalue)
{
    atomic_begin();

    task_t* task = task_getbyid(task_id);

    if (task != NULL)
    {
        if (task->state == TASK_STATE_CANCELED)
        {
            if (exitvalue != NULL)
            {
                *exitvalue = task->exitvalue;
            }

            atomic_end();
        }
        else
        {
            running->wait.task.task_handle = task->id;
            task_setstate(running, TASK_STATE_WAIT_TASK);
            
            atomic_end();
            
            sheduler_yield();

            if (exitvalue != NULL)
            {
                *exitvalue = running->wait.task.exitvalue;
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

/* --- Task stopping and canceling ---------------------------------------- */

bool task_cancel(task_t* task, int exitvalue)
{
    atomic_begin();

    if (task != NULL)
    {
        // Set the new task state
        task->exitvalue = exitvalue;
        task_setstate(task, TASK_STATE_CANCELED);

        log(LOG_DEBUG, "Task(%d) got canceled.", task->id);

        // Wake up waiting tasks
        list_foreach(i, task_bystate(TASK_STATE_WAIT_TASK))
        {
            task_t *waittask = i->value;

            if (waittask->wait.task.task_handle == task->id)
            {
                waittask->wait.task.exitvalue = exitvalue;
                task_setstate(waittask, TASK_STATE_RUNNING);

                log(LOG_DEBUG, "Task %d finish waiting task %d.", waittask->id, task->id);
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

void task_exit(int exitvalue)
{
    task_cancel(running, exitvalue);

    sheduler_yield();

    PANIC("sheduler_yield return but the task is canceled!");
}

/* --- Task Memory managment ---------------------------------------------- */

int task_memory_map(task_t* this, uint addr, uint count)
{
    return memory_map(this->pdir, addr, count, 1);
}

int task_memory_unmap(task_t* this, uint addr, uint count)
{
    return memory_unmap(this->pdir, addr, count);
}

uint task_memory_alloc(task_t* this, uint count)
{
    uint addr = memory_alloc(this->pdir, count, 1);
    log(LOG_DEBUG, "Gived userspace %d memory block at 0x%08x", count, addr);
    return addr;
}

void task_memory_free(task_t* this, uint addr, uint count)
{
    log(LOG_DEBUG, "Userspace free'd %d memory block at 0x%08x", count, addr);
    return memory_free(this->pdir, addr, count, 1);
}

/* --- File descriptor allocation and locking ------------------------------- */

void task_filedescriptor_close_all(task_t* this)
{
    for (int i = 0; i < TASK_FILDES_COUNT; i++)
    {
        if (this->fds[i].stream != NULL)
        {
            task_close_file(this, i);
        }
    }
}

int task_filedescriptor_alloc_and_acquire(task_t *this, stream_t *stream)
{
    lock_acquire(this->fds_lock);

    for (int i = 0; i < TASK_FILDES_COUNT; i++)
    {
        filedescriptor_t *fd = &this->fds[i];

        if (fd->free)
        {
            fd->free = false;
            fd->stream = stream;
            lock_acquire(fd->lock);

            lock_release(this->fds_lock);

            log(LOG_DEBUG, "File descriptor %d allocated for task %d", i, this->id);

            return i;
        }
    }

    lock_release(this->fds_lock);
    log(LOG_WARNING, "We run out of file descriptor on task %d", this->id);

    return -ERR_TOO_MANY_OPEN_FILES;
}

stream_t *task_filedescriptor_acquire(task_t *this, int fd_index)
{
    if (fd_index >= 0 && fd_index < TASK_FILDES_COUNT)
    {
        filedescriptor_t *fd = &this->fds[fd_index];
        lock_acquire(fd->lock);

        if (!fd->free)
        {
            return fd->stream;
        }
    }

    log(LOG_WARNING, "Got a bad file descriptor %d from task %d", fd_index, this->id);

    return NULL;
}

int task_filedescriptor_release(task_t *this, int fd_index)
{
    if (fd_index >= 0 && fd_index < TASK_FILDES_COUNT)
    {
        filedescriptor_t *fd = &this->fds[fd_index];

        lock_release(fd->lock);

        return 0;
    }

    log(LOG_WARNING, "Got a bad file descriptor %d from task %d", fd_index, this->id);

    return -ERR_BAD_FILE_DESCRIPTOR;
}

int task_filedescriptor_free_and_release(task_t *this, int fd_index)
{
    if (fd_index >= 0 && fd_index < TASK_FILDES_COUNT)
    {
        filedescriptor_t *fd = &this->fds[fd_index];

        lock_release(fd->lock);

        fd->free = true;
        fd->stream = NULL;

        log(LOG_DEBUG, "File descriptor %d free for task %d", fd_index, this->id);

        return 0;
    }

    log(LOG_WARNING, "Got a bad file descriptor %d from task %d", fd_index, this->id);

    return -ERR_BAD_FILE_DESCRIPTOR;
}


/* --- task file operations ----------------------------------------------- */

int task_open_file(task_t* this, const char *file_path, iostream_flag_t flags)
{
    path_t *p = task_cwd_resolve(this, file_path);

    stream_t *stream = filesystem_open(NULL, p, flags);

    path_delete(p);

    if (stream == NULL)
    {
        return -ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    int fd = task_filedescriptor_alloc_and_acquire(this, stream);

    if (fd >= 0)
    {
        task_filedescriptor_release(this, fd);
    }
    else
    {
        filesystem_close(stream);
    }

    return fd;
}

int task_close_file(task_t* this, int fd)
{
    stream_t *stream = task_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    filesystem_close(stream);

    task_filedescriptor_free_and_release(this, fd);

    return 0;
}

int task_read_file(task_t *this, int fd, void *buffer, uint size)
{
    stream_t *stream = task_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = filesystem_read(stream, buffer, size);

    task_filedescriptor_release(this, fd);

    return result;
}

int task_write_file(task_t *this, int fd, const void *buffer, uint size)
{
    stream_t *stream = task_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_write(stream, buffer, size);

    task_filedescriptor_release(this, fd);

    return result;
}

int task_ioctl_file(task_t *this, int fd, int request, void *args)
{
    stream_t *stream = task_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = filesystem_ioctl(stream, request, args);

    task_filedescriptor_release(this, fd);

    return result;
}

int task_seek_file(task_t *this, int fd, int offset, iostream_whence_t whence)
{
    stream_t *stream = task_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = filesystem_seek(stream, offset, whence);

    task_filedescriptor_release(this, fd);

    return result;
}

int task_tell_file(task_t *this, int fd, iostream_whence_t whence)
{
    stream_t *stream = task_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = filesystem_tell(stream, whence);

    task_filedescriptor_release(this, fd);

    return result;
}

int task_fstat_file(task_t *this, int fd, iostream_stat_t *stat)
{
    stream_t *stream = task_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = filesystem_fstat(stream, stat);

    task_filedescriptor_release(this, fd);

    return result;
}

/* --- Task dump ---------------------------------------------------------- */

static char *TASK_STATES[] =
    {
        "HANG",
        "LAUNCHPAD"
        "RUNNING",
        "SLEEP",
        "WAIT",
        "WAIT_FOR_MESSAGE",
        "CANCELED",
};

void task_dump(task_t *t)
{
    atomic_begin();
    printf("\n\t . Task %d %s", t->id, t->name);
    printf("\n\t   State: %s", TASK_STATES[t->state]);
    printf("\n\t   User memory: ");
    memory_layout_dump(t->pdir, true);
    printf("\n");
    atomic_end();
}

void task_panic_dump(void)
{
    atomic_begin();

    printf("\n");
    printf("\n\tRunning task %d: '%s'", sheduler_running_id(), sheduler_running()->name);
    printf("\n");
    printf("\n\tTasks:");

    list_foreach(i, tasks)
    {
        task_t *t = i->value;
        task_dump(t);
    }

    atomic_end();
}

/* --- Process elf file loading --------------------------------------------- */

void load_elfseg(task_t *this, iostream_t *s, elf_program_t *program)
{
    log(LOG_DEBUG, "Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", program->offset, program->filesz, program->vaddr, program->memsz);

    if (program->vaddr >= 0x100000)
    {
        atomic_begin();

        // To avoid pagefault we need to switch page directorie.
        page_directorie_t *pdir = running->pdir;

        paging_load_directorie(this->pdir);
        task_memory_map(this, program->vaddr, PAGE_ALIGN(program->memsz) / PAGE_SIZE + PAGE_SIZE);
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

int task_exec(const char *executable_path, const char **argv)
{
    // Check if the file existe and open the file.
    iostream_t *s = iostream_open(executable_path, IOSTREAM_READ);

    if (s == NULL)
    {
        log(LOG_WARNING, "'%s' file not found, exec failed!", executable_path);
        return -ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    // Check if the file isn't a directory.
    iostream_stat_t stat;
    iostream_fstat(s, &stat);

    if (stat.type != IOSTREAM_TYPE_REGULAR)
    {
        log(LOG_WARNING, "'%s' is not a file, exec failed!", executable_path);
        iostream_close(s);
        return -ERR_IS_A_DIRECTORY;
    }

    // Decode the elf file header.
    elf_header_t elf_header = {0};
    iostream_seek(s, 0, IOSTREAM_WHENCE_START);
    iostream_read(s, &elf_header, sizeof(elf_header_t));

    if (!elf_valid(&elf_header))
    {
        log(LOG_WARNING, "Invalid elf program!", executable_path);
        iostream_close(s);
        return -ERR_EXEC_FORMAT_ERROR;
    }

    // Create the process and load the executable.
    atomic_begin();

    task_t * new_task = task_spawn_with_argv(sheduler_running(), executable_path, (task_entry_t)elf_header.entry, argv, true);
    int new_task_id = new_task->id;

    elf_program_t program;

    for (int i = 0; i < elf_header.phnum; i++)
    {
        iostream_seek(s, elf_header.phoff + (elf_header.phentsize * i), IOSTREAM_WHENCE_START);
        iostream_read(s, &program, sizeof(elf_program_t));

        load_elfseg(new_task, s, &program);
    }

    log(LOG_DEBUG, "Executable loaded, creating main task...");

    task_go(new_task);

    log(LOG_DEBUG, "Process created, back to caller..");

    atomic_end();

    iostream_close(s);
    return new_task_id;
}

/* --- Current working directory -------------------------------------------- */

path_t* task_cwd_resolve(task_t* this, const char* path_to_resolve)
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

int task_set_cwd(task_t *this, const char *new_path)
{
    log(LOG_DEBUG, "Process %d set cwd to '%s'", this->id, new_path);
    
    path_t *work_path = task_cwd_resolve(this, new_path);
    
    lock_acquire(this->cwd_lock);

    fsnode_t *new_cwd = filesystem_acquire(NULL, work_path, false);

    if (new_cwd != NULL)
    {
        if (fsnode_to_iostream_type(new_cwd->type) == IOSTREAM_TYPE_DIRECTORY)
        {
            // Cleanup the old path
            path_delete(this->cwd_path);
            filesystem_release(this->cwd_node);

            // Set the new path
            this->cwd_node = new_cwd;
            this->cwd_path = work_path;

            lock_release(this->cwd_lock);

            return 0;
        }
        else
        {
            lock_release(this->cwd_lock);
            return -ERR_NOT_A_DIRECTORY;
        }
    }
    else
    {
        if (new_cwd != NULL)
        {
            filesystem_release(new_cwd);
        }

        path_delete(work_path);
        lock_release(this->cwd_lock);

        return -ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }
}

void task_get_cwd(task_t *this, char *buffer, uint size)
{
    lock_acquire(this->cwd_lock);

    path_to_cstring(this->cwd_path, buffer, size);

    lock_release(this->cwd_lock);
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
    strlcpy(channel->name, name, CHANNAME_SIZE);

    return channel;
}

void channel_delete(channel_t *channel)
{
    list_delete(channel->subscribers, LIST_KEEP_VALUES);
    free(channel);
}

channel_t *channel_get(const char *channel_name)
{
    list_foreach(i, channels)
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
    strlcpy(message->label, label, MSGLABEL_SIZE);

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

int messaging_send_internal(task_t* from, task_t*  to, int id, const char *name, void *payload, uint size, uint flags)
{
    if (to == NULL)
    {
        return 0;
    }

    if (to->inbox->count > 1024)
    {
        log(LOG_WARNING, "PROC=%d inbox is full!", to);
        return 0;
    }

    message_t *msg = message(id, name, payload, size, flags);

    msg->from = from->id;
    msg->to = to->id;

    list_pushback(to->inbox, (void *)msg);

    if (to->state == TASK_STATE_WAIT_MESSAGE)
    {
        messaging_receive_internal(to);
        task_setstate(to, TASK_STATE_RUNNING);
    }

    return id;
}

int messaging_send(task_t* to, const char *name, void *payload, uint size, uint flags)
{
    int id = 0;

    ATOMIC({
        id = messaging_send_internal(sheduler_running(), to, messaging_id(), name, payload, size, flags);
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

        list_foreach(p, c->subscribers)
        {
            messaging_send_internal(sheduler_running(), ((task_t *)p->value), id, name, payload, size, flags);
        }
    }

    atomic_end();

    return id;
}

message_t *messaging_receive_internal(task_t *task)
{
    message_t *msg = NULL;

    atomic_begin();

    if (task->inbox->count > 0)
    {
        if (task->wait.message.message != NULL)
        {
            message_delete(task->wait.message.message);
        }

        list_pop(task->inbox, (void **)&msg);
        task->wait.message.message = msg;
    }

    atomic_end();
    return msg;
}

bool messaging_receive(message_t *msg, bool wait)
{
    message_t *incoming = messaging_receive_internal(sheduler_running());

    if (incoming == NULL && wait)
    {
        atomic_begin();
        task_setstate(sheduler_running(), TASK_STATE_WAIT_MESSAGE);
        atomic_end();

        sheduler_yield(); // Wait until we get a message.

        incoming = sheduler_running()->wait.message.message;
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
    message_t *incoming = sheduler_running()->wait.message.message;

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

    list_pushback(c->subscribers, sheduler_running());

    atomic_end();

    return 0;
}

int messaging_unsubscribe(const char *channel_name)
{
    atomic_begin();

    channel_t *c = channel_get(channel_name);

    if (c != NULL)
    {
        list_remove(c->subscribers, sheduler_running());
    }

    atomic_end();

    return 0;
}

/* -------------------------------------------------------------------------- */
/*   GARBAGE COLECTOR                                                         */
/* -------------------------------------------------------------------------- */

void collect_and_free_task(void)
{
    list_t *task_to_free = list();

    atomic_begin();
    // Get canceled tasks
    list_foreach(i, task_bystate(TASK_STATE_CANCELED))
    {
        task_t *task = i->value;
        list_pushback(task_to_free, task);
    }

    atomic_end();

    // Cleanup all of those dead tasks.
    list_foreach(i, task_to_free)
    {
        task_t *task = i->value;
        int task_id = task->id;

        log(LOG_DEBUG, "free'ing task %d", task_id);

        task_delete(task);

        log(LOG_DEBUG, "Task %d free'd.", task_id);
    }

    list_delete(task_to_free, LIST_KEEP_VALUES);
}

void garbage_colector()
{
    while (true)
    {
        task_sleep(100); // We don't do this really often.

        // log(LOG_DEBUG, "Garbage collect begin %d !", ticks);
        collect_and_free_task();

        // log(LOG_DEBUG, "Garbage collect end!");
    }
}

/* -------------------------------------------------------------------------- */
/*   SHEDULER                                                                 */
/* -------------------------------------------------------------------------- */

static bool sheduler_context_switch = false;
#define SHEDULER_RECORD_COUNT 1024
static int sheduler_record[SHEDULER_RECORD_COUNT] = {0};

void timer_set_frequency(int hz)
{
    u32 divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    log(LOG_DEBUG, "Timer frequency is %dhz.", hz);
}

void sheduler_setup(task_t *main_kernel_task)
{
    running = main_kernel_task;

    timer_set_frequency(100);
    irq_register(0, (irq_handler_t)&shedule);
}

/* --- Sheduling ------------------------------------------------------------ */

void wakeup_sleeping_tasks(void)
{
    if (!list_empty(task_bystate(TASK_STATE_WAIT_TIME)))
    {
        task_t *t;

        do
        {
            if (list_peek(task_bystate(TASK_STATE_WAIT_TIME), (void **)&t))
            {
                if (t->wait.time.wakeuptick <= ticks)
                {
                    task_setstate(t, TASK_STATE_RUNNING);
                    log(LOG_DEBUG, "Task %d wake up!", t->id);
                }
            }

        } while (t != NULL && t->state == TASK_STATE_RUNNING);
    }
}

#include <hjert/dev/vga.h>

static const char *animation = "|/-\\|/-\\";

reg32_t shedule(reg32_t sp, processor_context_t *context)
{
    //log(LOG_DEBUG, "Current task %d(%s) with eip@%08x.", running->id, running->process->name, context->eip);
    UNUSED(context);
    sheduler_context_switch = true;

    // Save the old context
    running->sp = sp;

    // Update the system ticks
    ticks++;
    wakeup_sleeping_tasks();

    // Get the next task
    if (!list_peek_and_pushback(task_bystate(TASK_STATE_RUNNING), (void **)&running))
    {
        // Or the idle task if there are no running tasks.
        running = idle_task;
    }

    sheduler_record[ticks % SHEDULER_RECORD_COUNT] = running->id;

    vga_cell(VGA_SCREEN_WIDTH - 1, VGA_SCREEN_HEIGHT - 1, VGA_ENTRY(animation[(ticks / 100) % sizeof(animation)], VGACOLOR_WHITE, (running == idle_task) ? VGACOLOR_BLACK : VGACOLOR_GREEN));

    // Restore the context
    // TODO: set_kernel_stack(...);
    paging_load_directorie(running->pdir);
    paging_invalidate_tlb();

    sheduler_context_switch = false;

    //log(LOG_DEBUG, "Now current task is %d(%s)", running->id, running->process->name);

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

/* --- Running task info -------------------------------------------------- */

task_t *sheduler_running(void)
{
    return running;
}

int sheduler_running_id(void)
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

int sheduler_get_usage(int task_id)
{
    int count = 0;

    atomic_begin();
    for (int i = 0; i < SHEDULER_RECORD_COUNT; i++)
    {
        if (sheduler_record[i] == task_id)
        {
            count++;
        }
    }
    atomic_end();
    
    return count;
}
