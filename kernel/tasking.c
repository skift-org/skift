/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/atomic.h>
#include <libsystem/error.h>
#include <libfile/elf.h>
#include <libmath/math.h>

#include "x86/irq.h"
#include "tasking.h"
#include "platform.h"

/* -------------------------------------------------------------------------- */
/*   TASKING                                                                  */
/* -------------------------------------------------------------------------- */

static uint ticks = 0;
static task_t *running = NULL;

static task_t *kernel_task;
task_t *task_kernel(void) { return kernel_task; }

static task_t *garbage_task;
static task_t *idle_task;

void idle_code() { HANG; }

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

task_t *task(task_t *parent, const char *name, bool user)
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
    this->inbox = list();
    this->subscription = list();

    // Setup shms
    this->shms = list();

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

    // setup the stack
    memset(this->stack, 0, TASK_STACKSIZE);
    this->sp = (reg32_t)(&this->stack[0] + TASK_STACKSIZE - 1);
    platform_fpu_save_context(this);

    return this;
}

void task_delete(task_t *this)
{
    atomic_begin();
    if (this->state != TASK_STATE_NONE)
        task_setstate(this, TASK_STATE_NONE);

    list_remove(tasks, this);
    atomic_end();

    list_delete(this->inbox, LIST_FREE_VALUES);
    list_delete(this->subscription, LIST_FREE_VALUES);
    list_delete(this->shms, LIST_RELEASE_VALUES);

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

list_t *task_all(void)
{
    return tasks;
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

void task_get_info(task_t *this, task_info_t *info)
{
    assert(this);

    info->id = this->id;
    info->state = this->state;

    strlcpy(info->name, this->name, TASK_NAMESIZE);
    path_to_cstring(this->cwd_path, info->cwd, PATH_LENGHT);

    info->usage_cpu = (sheduler_get_usage(this->id) * 100) / SHEDULER_RECORD_COUNT;
}

int task_count(void)
{
    atomic_begin();
    int result = list_count(tasks);
    atomic_end();

    return result;
}

task_t *task_spawn(task_t *parent, const char *name, task_entry_t entry, void *arg, bool user)
{
    ASSERT_ATOMIC;

    task_t *t = task(parent, name, user);

    task_setentry(t, entry, user);
    task_stack_push(t, &arg, sizeof(arg));

    return t;
}

task_t *task_spawn_with_argv(task_t *parent, const char *name, task_entry_t entry, const char **argv, bool user)
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

    atomic_begin();
    task_setstate(t, TASK_STATE_RUNNING);
    atomic_end();
}

/* --- Task wait state ---------------------------------------------------- */

task_sleep_result_t task_sleep(task_t *this, int timeout)
{
    ATOMIC({
        this->wait.time.wakeuptick = ticks + timeout;
        this->wait.time.gotwakeup = false;

        task_setstate(this, TASK_STATE_WAIT_TIME);
    });

    sheduler_yield();

    if (this->wait.time.gotwakeup)
    {
        return TASK_SLEEP_RESULT_WAKEUP;
    }
    else
    {
        return TASk_SLEEP_RESULT_TIMEOUT;
    }
}

int task_wakeup(task_t *this)
{
    ASSERT_ATOMIC;

    if (this != NULL && this->state == TASK_STATE_WAIT_TIME)
    {
        this->wait.time.gotwakeup = true;
        this->wait.time.wakeuptick = 0;

        task_setstate(this, TASK_STATE_RUNNING);

        return 0;
    }

    return -1;
}

bool task_wait(int task_id, int *exitvalue)
{
    atomic_begin();

    task_t *task = task_getbyid(task_id);

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

bool task_wait_stream(task_t *this, stream_t *stream, task_wait_stream_condition_t condition)
{
    atomic_begin();

    if (!lock_is_acquire(stream->node->lock) && condition(stream))
    {
        lock_acquire(stream->node->lock);
    }
    else
    {
        task_setstate(this, TASK_STATE_WAIT_STREAM);
        this->wait.stream.stream = stream;
        this->wait.stream.condition = condition;
    }

    atomic_end();

    sheduler_yield();

    return true;
}

/* --- Task stopping and canceling ---------------------------------------- */

bool task_cancel(task_t *task, int exitvalue)
{
    atomic_begin();

    if (task != NULL)
    {
        // Set the new task state
        task->exitvalue = exitvalue;
        task_setstate(task, TASK_STATE_CANCELED);

        // Wake up waiting tasks
        list_foreach(i, task_bystate(TASK_STATE_WAIT_TASK))
        {
            task_t *waittask = i->value;

            if (waittask->wait.task.task_handle == task->id)
            {
                waittask->wait.task.exitvalue = exitvalue;
                task_setstate(waittask, TASK_STATE_RUNNING);
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

page_directorie_t *task_switch_pdir(task_t *task, page_directorie_t *pdir)
{
    page_directorie_t *oldpdir = task->pdir;

    task->pdir = pdir;
    paging_load_directorie(pdir);

    return oldpdir;
}

int task_memory_map(task_t *this, uint addr, uint count)
{
    return memory_map(this->pdir, addr, count, 1);
}

int task_memory_unmap(task_t *this, uint addr, uint count)
{
    return memory_unmap(this->pdir, addr, count);
}

uint task_memory_alloc(task_t *this, uint count)
{
    uint addr = memory_alloc(this->pdir, count, 1);
    return addr;
}

void task_memory_free(task_t *this, uint addr, uint count)
{
    return memory_free(this->pdir, addr, count, 1);
}

/* --- File descriptor allocation and locking ------------------------------- */

void task_filedescriptor_close_all(task_t *this)
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

            return i;
        }
    }

    lock_release(this->fds_lock);

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

    logger_warn("Got a bad file descriptor %d from task %d", fd_index, this->id);

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

    logger_warn("Got a bad file descriptor %d from task %d", fd_index, this->id);

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

        return 0;
    }

    logger_warn("Got a bad file descriptor %d from task %d", fd_index, this->id);

    return -ERR_BAD_FILE_DESCRIPTOR;
}

/* --- task file operations ----------------------------------------------- */

int task_open_file(task_t *this, const char *file_path, iostream_flag_t flags)
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

int task_close_file(task_t *this, int fd)
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

int task_call_file(task_t *this, int fd, int request, void *args)
{
    stream_t *stream = task_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = filesystem_call(stream, request, args);

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

int task_stat_file(task_t *this, int fd, iostream_stat_t *stat)
{
    stream_t *stream = task_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = filesystem_stat(stream, stat);

    task_filedescriptor_release(this, fd);

    return result;
}

/* --- Task dump ---------------------------------------------------------- */

static char *TASK_STATES[] = {
    "HANG",
    "LAUNCHPAD",
    "RUNNING",
    "SLEEP",
    "WAIT",
    "WAIT_FOR_MESSAGE",
    "WAIT_FOR_RESPOND",
    "WAIT_FOR_STREAM",
    "CANCELED",
};

void task_dump(task_t *t)
{
    atomic_begin();
    printf("\n\t - Task %d %s", t->id, t->name);
    printf("\n\t   State: %s", TASK_STATES[t->state]);
    printf("\n\t   User memory: ");
    memory_layout_dump(t->pdir, true);
    printf("\n\t   Page directory: %08x", t->pdir);

    printf("\n");
    atomic_end();
}

void task_panic_dump(void)
{
    atomic_begin();

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
    if (program->vaddr >= 0x100000)
    {
        // To avoid pagefault we need to switch page directorie.
        page_directorie_t *oldpdir = task_switch_pdir(sheduler_running(), this->pdir);

        paging_load_directorie(this->pdir);
        task_memory_map(this, program->vaddr, PAGE_ALIGN_UP(program->memsz) / PAGE_SIZE);
        memset((void *)program->vaddr, 0, program->memsz);

        iostream_seek(s, program->offset, IOSTREAM_WHENCE_START);
        iostream_read(s, (void *)program->vaddr, program->filesz);

        task_switch_pdir(sheduler_running(), oldpdir);
    }
    else
    {
        logger_warn("Elf segment ignored, not in user memory!");
    }
}

int task_exec(const char *executable_path, const char **argv)
{
    // Check if the file existe and open the file.
    iostream_t *s = iostream_open(executable_path, IOSTREAM_READ | IOSTREAM_BUFFERED_NONE);

    if (s == NULL)
    {
        logger_warn("'%s' file not found, exec failed!", executable_path);
        return -ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    // Check if the file isn't a directory.
    iostream_stat_t stat;
    iostream_stat(s, &stat);

    if (stat.type != IOSTREAM_TYPE_REGULAR)
    {
        logger_warn("'%s' is not a file, exec failed!", executable_path);
        iostream_close(s);
        return -ERR_IS_A_DIRECTORY;
    }

    logger_info("Loading elf file %s...", executable_path);

    // Decode the elf file header.
    elf_header_t elf_header = {0};
    iostream_seek(s, 0, IOSTREAM_WHENCE_START);
    iostream_read(s, &elf_header, sizeof(elf_header_t));

    if (!elf_valid(&elf_header))
    {
        logger_warn("Invalid elf program!", executable_path);
        iostream_close(s);
        return -ERR_EXEC_FORMAT_ERROR;
    }

    // Create the process and load the executable.
    task_t *new_task = task_spawn_with_argv(sheduler_running(), executable_path, (task_entry_t)elf_header.entry, argv, true);
    int new_task_id = new_task->id;

    elf_program_t program;

    for (int i = 0; i < elf_header.phnum; i++)
    {
        iostream_seek(s, elf_header.phoff + (elf_header.phentsize * i), IOSTREAM_WHENCE_START);
        
        int readed_bytes = iostream_read(s, &program, sizeof(elf_program_t));
        logger_trace("Readed %08x bytes from the elf file.", readed_bytes);

        logger_trace("Loading elf program section at offset %d and size %d", program.offset, program.filesz);

        load_elfseg(new_task, s, &program);
    }

    task_go(new_task);

    iostream_close(s);
    return new_task_id;
}

/* --- Current working directory -------------------------------------------- */

path_t *task_cwd_resolve(task_t *this, const char *path_to_resolve)
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
    path_t *work_path = task_cwd_resolve(this, new_path);

    lock_acquire(this->cwd_lock);

    fsnode_t *new_cwd = filesystem_acquire(NULL, work_path, false);

    if (new_cwd != NULL)
    {
        if (fsnode_type(new_cwd) == IOSTREAM_TYPE_DIRECTORY)
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
/*   SHARED MEMORY                                                            */
/* -------------------------------------------------------------------------- */

static int SHMID = 0;
static lock_t shms_lock;
static list_t *shms;

void task_shared_memory_setup(void)
{
    shms = list();
    lock_init(shms_lock);
}

/* --- Shared phycical region ----------------------------------------------- */

void shm_physical_region_delete(shm_physical_region_t *this);
shm_physical_region_t *shm_physical_region(int pagecount)
{
    shm_physical_region_t *this = OBJECT(shm_physical_region);

    this->ID = SHMID++;
    this->paddr = physical_alloc(pagecount);
    this->pagecount = pagecount;

    if (this->paddr)
    {
        list_pushback(shms, this);

        return this;
    }
    else
    {
        object_release(this);

        return NULL;
    }
}

void shm_physical_region_delete(shm_physical_region_t *this)
{
    list_remove(shms, this);
    physical_free(this->paddr, this->pagecount);
}

shm_physical_region_t *task_physical_region_get_by_id(int id)
{
    if (id < SHMID)
        return NULL;

    list_foreach(i, shms)
    {
        shm_physical_region_t *shm = (shm_physical_region_t *)i->value;

        if (shm->ID == id)
            return shm;
    }

    return NULL;
}

/* --- Shared virtual region ------------------------------------------------ */
void shm_virtual_region_delete(shm_virtual_region_t *this);
shm_virtual_region_t *shm_virtual_region(task_t *task, shm_physical_region_t *physr)
{
    uint vaddr = virtual_alloc(task->pdir, physr->paddr, physr->pagecount, 1);

    if (vaddr == 0)
    {
        return NULL;
    }

    shm_virtual_region_t *virtr = OBJECT(shm_virtual_region);
    virtr->region = object_retain(physr);
    virtr->vaddr = vaddr;

    return virtr;
}

void shm_virtual_region_delete(shm_virtual_region_t *this)
{
    object_release(this->region);

    if (object_refcount(this->region) == 1)
    {
        // We were the last task to have a ref to this shm
        // The last ref is the shm list, so we can release that too

        object_release(this->region);
    }
}

shm_virtual_region_t *task_virtual_region_get_by_id(task_t *this, int id)
{
    if (id < SHMID)
        return NULL;

    list_foreach(i, this->shms)
    {
        shm_virtual_region_t *shm = (shm_virtual_region_t *)i->value;

        if (shm->region->ID == id)
            return shm;
    }

    return NULL;
}

/* --- User facing API ------------------------------------------------------ */

int task_shared_memory_alloc(task_t *this, int pagecount)
{
    lock_acquire(shms_lock);

    shm_physical_region_t *physr = shm_physical_region(pagecount);

    if (physr == NULL)
    {
        return -ERR_CANNOT_ALLOCATE_MEMORY;
    }

    shm_virtual_region_t *virtr = shm_virtual_region(this, physr);

    if (virtr == NULL)
    {
        lock_release(shms_lock);
        return -ERR_CANNOT_ALLOCATE_MEMORY;
    }

    lock_release(shms_lock);
    return physr->ID;
}

int task_shared_memory_acquire(task_t *this, int shm, uint *addr)
{
    lock_acquire(shms_lock);

    shm_virtual_region_t *virtr = task_virtual_region_get_by_id(this, shm);

    if (virtr != NULL)
    {
        *addr = virtr->vaddr;
        lock_release(shms_lock);
        return -ERR_SUCCESS;
    }

    shm_physical_region_t *physr = task_physical_region_get_by_id(shm);

    if (physr == NULL)
    {
        lock_release(shms_lock);
        return -ERR_BAD_ADDRESS; // FIXME: create a better error for this
    }

    /* shm_virtual_region_t* */ virtr = shm_virtual_region(this, physr);

    if (virtr == NULL)
    {
        lock_release(shms_lock);
        return -ERR_CANNOT_ALLOCATE_MEMORY;
    }

    *addr = virtr->vaddr;
    lock_release(shms_lock);
    return -ERR_SUCCESS;
}

int task_shared_memory_release(task_t *this, int shm)
{
    lock_acquire(shms_lock);

    shm_virtual_region_t *virtr = task_virtual_region_get_by_id(this, shm);

    if (virtr == NULL)
    {
        lock_release(shms_lock);
        return -ERR_BAD_ADDRESS; // FIXME: create a better error for this
    }
    else
    {
        object_release(virtr);
        lock_release(shms_lock);
        return -ERR_SUCCESS;
    }
}

/* -------------------------------------------------------------------------- */
/*   MESSAGING                                                                */
/* -------------------------------------------------------------------------- */

static int MID = 0;

/* --- Channels ------------------------------------------------------------- */

bool task_messaging_has_subscribe(task_t *this, const char *channel)
{
    atomic_begin();

    list_foreach(i, this->subscription)
    {
        const char *subscription = (char *)i->value;

        if (strcmp(channel, subscription) == 0)
        {
            atomic_end();

            return true;
        }
    }

    atomic_end();

    return false;
}

int task_messaging_subscribe(task_t *this, const char *channel)
{
    if (task_messaging_has_subscribe(this, channel))
    {
        return -ERR_SUCCESS;
    }
    else
    {
        atomic_begin();

        list_pushback(this->subscription, strdup(channel));

        atomic_end();
    }

    return -ERR_SUCCESS;
}

int task_messaging_unsubscribe(task_t *this, const char *channel)
{
    if (task_messaging_has_subscribe(this, channel))
    {
        return -ERR_SUCCESS;
    }
    else
    {
        atomic_begin();

        list_foreach(i, this->subscription)
        {
            char *subscription = (char *)i->value;

            if (strcmp(channel, subscription) == 0)
            {
                list_remove(this->subscription, subscription);
                break;
            }
        }

        atomic_end();
    }

    return -ERR_SUCCESS;
}

/* --- Messages ------------------------------------------------------------- */

int task_messaging_send_internal(task_t *this, task_t *destination, message_t *event, message_type_t message_type)
{
    ASSERT_ATOMIC;

    if (!(list_count(destination->inbox) < 1024))
    {
        return -ERR_INBOX_FULL;
    }

    message_t *event_copy = MALLOC(message_t);
    *event_copy = *event;

    event_copy->header.from = this->id;
    event_copy->header.to = destination->id;
    event_copy->header.type = message_type;

    list_pushback(destination->inbox, event_copy);

    if (destination->state == TASK_STATE_WAIT_MESSAGE)
    {
        task_setstate(destination, TASK_STATE_RUNNING);
    }

    return -ERR_SUCCESS;
}

int task_messaging_send(task_t *this, message_t *event)
{
    atomic_begin();

    event->header.id = MID++;

    task_t *destination = task_getbyid(event->header.to);

    if (destination == NULL)
    {
        atomic_end();
        return -ERR_NO_SUCH_PROCESS;
    }

    int result = task_messaging_send_internal(this, destination, event, MESSAGE_TYPE_EVENT);

    atomic_end();

    return result;
}

int task_messaging_broadcast(task_t *this, const char *channel, message_t *event)
{
    atomic_begin();

    event->header.id = MID++;

    list_foreach(i, task_all())
    {
        task_t *destination = (task_t *)i->value;

        if (destination != this && task_messaging_has_subscribe(destination, channel))
        {
            task_messaging_send_internal(this, destination, event, MESSAGE_TYPE_EVENT);
        }
    }

    atomic_end();

    return -ERR_SUCCESS;
}

int task_messaging_request(task_t *this, message_t *request, message_t *respond, int timeout)
{
    atomic_begin();
    {
        request->header.id = MID++;

        task_t *destination = task_getbyid(request->header.to);

        if (destination == NULL)
        {
            atomic_end();
            return -ERR_NO_SUCH_PROCESS;
        }

        this->wait.respond = (task_wait_respond_t){
            .has_result = false,
            .result = {},
            .timeout = timeout,
        };

        task_messaging_send_internal(this, destination, request, MESSAGE_TYPE_REQUEST);

        task_setstate(this, TASK_STATE_WAIT_RESPOND);
    }
    atomic_end();

    sheduler_yield();

    if (this->wait.respond.has_result)
    {
        *respond = this->wait.respond.result;

        return -ERR_SUCCESS;
    }
    else
    {
        *respond = (message_t){0};
        return -ERR_REQUEST_TIMEOUT;
    }
}

int task_messaging_respond(task_t *this, message_t *request, message_t *result)
{
    atomic_begin();

    task_t *destination = task_getbyid(request->header.from);
    if (destination == NULL || destination->state != TASK_STATE_WAIT_RESPOND)
    {
        atomic_end();
        return -ERR_NO_SUCH_PROCESS;
    }

    // FIXME: this is a lot of dot operator...
    destination->wait.respond.has_result = true;
    destination->wait.respond.result = *result;

    destination->wait.respond.result.header.from = this->id;
    destination->wait.respond.result.header.to = destination->id;

    task_setstate(destination, TASK_STATE_RUNNING);

    atomic_end();

    return -ERR_SUCCESS;
}

int task_messaging_receive(task_t *this, message_t *message, bool wait)
{
    message_t *received = NULL;

    atomic_begin();

    if (list_pop(this->inbox, (void **)&received))
    {
        *message = *received;
    }

    if (received == NULL && wait)
    {
        task_setstate(this, TASK_STATE_WAIT_MESSAGE);

        atomic_end();

        sheduler_yield();

        atomic_begin();

        if (list_pop(this->inbox, (void **)&received))
        {
            *message = *received;
        }
    }

    atomic_end();

    if (received == NULL)
    {
        return -ERR_NO_MESSAGE;
    }
    else
    {
        free(received);
        return -ERR_SUCCESS;
    }
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
        task_delete((task_t *)i->value);
    }

    list_delete(task_to_free, LIST_KEEP_VALUES);
}

void garbage_colector()
{
    while (true)
    {
        task_sleep(sheduler_running(), 100); // We don't do this really often.
        collect_and_free_task();
    }
}

/* -------------------------------------------------------------------------- */
/*   SHEDULER                                                                 */
/* -------------------------------------------------------------------------- */

static bool sheduler_context_switch = false;
static int sheduler_record[SHEDULER_RECORD_COUNT] = {0};

void timer_set_frequency(int hz)
{
    u32 divisor = 1193180 / hz;
    out8(0x43, 0x36);
    out8(0x40, divisor & 0xFF);
    out8(0x40, (divisor >> 8) & 0xFF);

    logger_info("Timer frequency is %dhz.", hz);
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
                }
            }

        } while (t != NULL && t->state == TASK_STATE_RUNNING);
    }
}

void wakeup_stream_waiting_task(void)
{
    if (!list_empty(task_bystate(TASK_STATE_WAIT_STREAM)))
    {
        list_t *task_to_wakeup = list();

        list_foreach(t, task_bystate(TASK_STATE_WAIT_STREAM))
        {
            task_t *task = t->value;
            stream_t *stream = task->wait.stream.stream;

            if (!lock_is_acquire(stream->node->lock) &&
                task->wait.stream.condition(stream))
            {
                lock_acquire_by(stream->node->lock, task->id);
                list_pushback(task_to_wakeup, task);
            }
        }

        list_foreach(t, task_to_wakeup)
        {
            task_t *task = t->value;
            task_setstate(task, TASK_STATE_RUNNING);
        }

        list_delete(task_to_wakeup, LIST_KEEP_VALUES);
    }
}

reg32_t shedule(reg32_t sp, processor_context_t *context)
{
    UNUSED(context);
    sheduler_context_switch = true;

    // Save the old context
    running->sp = sp;
    platform_save_context(running);

    // Update the system ticks
    sheduler_record[ticks % SHEDULER_RECORD_COUNT] = running->id;
    ticks++;

    wakeup_sleeping_tasks();
    wakeup_stream_waiting_task();

    // Get the next task
    if (!list_peek_and_pushback(task_bystate(TASK_STATE_RUNNING), (void **)&running))
    {
        // Or the idle task if there are no running tasks.
        running = idle_task;
    }

    // Restore the context
    // TODO: set_kernel_stack(...);
    paging_load_directorie(running->pdir);
    paging_invalidate_tlb();

    sheduler_context_switch = false;

    platform_load_context(running);
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
