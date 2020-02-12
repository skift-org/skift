/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libfile/elf.h>
#include <libmath/math.h>
#include <libsystem/assert.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/debug.h>
#include <libsystem/error.h>

#include "platform.h"
#include "tasking.h"
#include "tasking/Handles.h"

/* -------------------------------------------------------------------------- */
/*   TASKING                                                                  */
/* -------------------------------------------------------------------------- */

static int TID = 1;
static List *tasks;
static List *tasks_bystates[TASK_STATE_COUNT];
static uint ticks = 0;
static Task *running = NULL;

static Task *kernel_task;
static Task *garbage_task;
static Task *idle_task;

void idle_code() { HANG; }

void tasking_initialize(void)
{
    tasks = list_create();

    for (int i = 0; i < TASK_STATE_COUNT; i++)
    {
        tasks_bystates[i] = list_create();
    }

    running = NULL;

    kernel_task = task_spawn(NULL, "System", NULL, NULL, 0);
    task_go(kernel_task);

    garbage_task = task_spawn(kernel_task, "GarbageColector", garbage_colector, NULL, false);
    task_go(garbage_task);

    idle_task = task_spawn(kernel_task, "Idle", idle_code, NULL, false);
    task_go(idle_task);
    task_set_state(idle_task, TASK_STATE_HANG);

    sheduler_setup(kernel_task);
}

/* -------------------------------------------------------------------------- */
/*   TASKS                                                                    */
/* -------------------------------------------------------------------------- */

Task *task_create(Task *parent, const char *name, bool user)
{
    ASSERT_ATOMIC;

    Task *task = __create(Task);

    task->id = TID++;
    strlcpy(task->name, name, PROCESS_NAME_SIZE);
    task->state = TASK_STATE_NONE;

    list_pushback(tasks, task);

    // Setup inbox
    task->inbox = list_create();
    task->subscription = list_create();

    // Setup shms
    task->shms = list_create();

    // Setup current working directory.
    lock_init(task->cwd_lock);

    if (parent != NULL)
    {
        task->cwd_path = path_clone(parent->cwd_path);
    }
    else
    {
        Path *path = path_create("/");
        task->cwd_path = path;
        assert(task->cwd_path);
    }

    // Setup fildes
    lock_init(task->handles_lock);
    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        task->handles[i] = NULL;
    }

    // Setup memory space
    if (user)
    {
        task->pdir = memory_alloc_pdir();
    }
    else
    {
        task->pdir = memory_kpdir();
    }

    // setup the stack
    memset(task->stack, 0, PROCESS_STACK_SIZE);
    task->stack_pointer = (uintptr_t)(&task->stack[0] + PROCESS_STACK_SIZE - 1);

    platform_fpu_save_context(task);

    return task;
}

void task_destroy(Task *task)
{
    atomic_begin();
    if (task->state != TASK_STATE_NONE)
        task_set_state(task, TASK_STATE_NONE);

    list_remove(tasks, task);
    atomic_end();

    list_destroy(task->inbox, LIST_FREE_VALUES);
    list_destroy(task->subscription, LIST_FREE_VALUES);
    list_destroy(task->shms, LIST_RELEASE_VALUES);

    task_fshandle_close_all(task);

    lock_acquire(task->cwd_lock);
    path_destroy(task->cwd_path);

    if (task->pdir != memory_kpdir())
    {
        memory_free_pdir(task->pdir);
    }

    free(task);
}

List *task_all(void)
{
    return tasks;
}

List *task_by_state(TaskState state)
{
    return tasks_bystates[state];
}

Task *task_by_id(int id)
{
    list_foreach(Task, task, tasks)
    {
        if (task->id == id)
            return task;
    }

    return NULL;
}

void task_get_info(Task *task, TaskInfo *info)
{
    assert(task);

    info->id = task->id;
    info->state = task->state;

    strlcpy(info->name, task->name, PROCESS_NAME_SIZE);
    path_to_cstring(task->cwd_path, info->cwd, PATH_LENGHT);

    info->usage_cpu = (sheduler_get_usage(task->id) * 100) / SHEDULER_RECORD_COUNT;
}

int task_count(void)
{
    atomic_begin();
    int result = list_count(tasks);
    atomic_end();

    return result;
}

Task *task_spawn(Task *parent, const char *name, TaskEntry entry, void *arg, bool user)
{
    ASSERT_ATOMIC;

    Task *t = task_create(parent, name, user);

    task_set_entry(t, entry, user);
    task_stack_push(t, &arg, sizeof(arg));

    return t;
}

Task *task_spawn_with_argv(Task *parent, const char *name, TaskEntry entry, const char **argv, bool user)
{
    atomic_begin();

    Task *t = task_create(parent, name, user);

    task_set_entry(t, entry, true);

    uint argv_list[PROCESS_ARG_COUNT] = {0};

    int argc;
    for (argc = 0; argv[argc] && argc < PROCESS_ARG_COUNT; argc++)
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
    return ((Task *)left)->wait.time.wakeuptick < ((Task *)right)->wait.time.wakeuptick;
}

void task_set_state(Task *task, TaskState state)
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

void task_set_entry(Task *t, TaskEntry entry, bool user)
{
    t->entry = entry;
    t->user = user;
}

uintptr_t task_stack_push(Task *task, const void *value, uint size)
{
    task->stack_pointer -= size;
    memcpy((void *)task->stack_pointer, value, size);

    return task->stack_pointer;
}

void task_go(Task *task)
{
    InterruptStackFrame stackframe;

    stackframe.eflags = 0x202;
    stackframe.eip = (uintptr_t)task->entry;
    stackframe.ebp = ((uintptr_t)task->stack + PROCESS_STACK_SIZE);

    // TODO: userspace task
    stackframe.cs = 0x08;
    stackframe.ds = 0x10;
    stackframe.es = 0x10;
    stackframe.fs = 0x10;
    stackframe.gs = 0x10;

    task_stack_push(task, &stackframe, sizeof(InterruptStackFrame));

    atomic_begin();
    task_set_state(task, TASK_STATE_RUNNING);
    atomic_end();
}

/* --- Task wait state ---------------------------------------------------- */

task_sleep_result_t task_sleep(Task *task, int timeout)
{
    ATOMIC({
        task->wait.time.wakeuptick = ticks + timeout;
        task->wait.time.gotwakeup = false;

        task_set_state(task, TASK_STATE_WAIT_TIME);
    });

    sheduler_yield();

    if (task->wait.time.gotwakeup)
    {
        return TASK_SLEEP_RESULT_WAKEUP;
    }
    else
    {
        return TASk_SLEEP_RESULT_TIMEOUT;
    }
}

int task_wakeup(Task *task)
{
    ASSERT_ATOMIC;

    if (task != NULL && task->state == TASK_STATE_WAIT_TIME)
    {
        task->wait.time.gotwakeup = true;
        task->wait.time.wakeuptick = 0;

        task_set_state(task, TASK_STATE_RUNNING);

        return 0;
    }

    return -1;
}

bool task_wait(int task_id, int *exitvalue)
{
    atomic_begin();

    Task *task = task_by_id(task_id);

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
            task_set_state(running, TASK_STATE_WAIT_TASK);

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

void task_block(Task *task, TaskBlocker *blocker)
{
    assert(!task->blocker);

    atomic_begin();
    task->blocker = blocker;
    task_set_state(task, TASK_STATE_BLOCKED);
    atomic_end();

    sheduler_yield();
}

/* --- Task stopping and canceling ---------------------------------------- */

bool task_cancel(Task *task, int exitvalue)
{
    atomic_begin();

    if (task != NULL)
    {
        // Set the new task state
        task->exitvalue = exitvalue;
        task_set_state(task, TASK_STATE_CANCELED);

        // Wake up waiting tasks
        list_foreach(Task, waittask, task_by_state(TASK_STATE_WAIT_TASK))
        {
            if (waittask->wait.task.task_handle == task->id)
            {
                waittask->wait.task.exitvalue = exitvalue;
                task_set_state(waittask, TASK_STATE_RUNNING);
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

    ASSERT_NOT_REACHED();
}

/* --- Task Memory managment ---------------------------------------------- */

page_directorie_t *task_switch_pdir(Task *task, page_directorie_t *pdir)
{
    page_directorie_t *oldpdir = task->pdir;

    task->pdir = pdir;

    paging_load_directorie(pdir);

    return oldpdir;
}

int task_memory_map(Task *task, uint addr, uint count)
{
    return memory_map(task->pdir, addr, count, 1);
}

int task_memory_unmap(Task *task, uint addr, uint count)
{
    return memory_unmap(task->pdir, addr, count);
}

uint task_memory_alloc(Task *task, uint count)
{
    return memory_alloc(task->pdir, count, 1);
}

void task_memory_free(Task *task, uint addr, uint count)
{
    return memory_free(task->pdir, addr, count, 1);
}

/* --- Task dump ---------------------------------------------------------- */

static char *TASK_STATES[] = {
    "HANG",
    "LAUNCHPAD",
    "RUNNING",
    "BLOCKED",
    "WAIT_TIME",
    "WAIT_TASK",
    "WAIT_MESSAGE",
    "WAIT_RESPOND",
    "CANCELED",
};

void task_dump(Task *t)
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
    if (running == NULL)
        return;

    atomic_begin();

    printf("\n\tRunning task %d: '%s'", sheduler_running_id(), sheduler_running()->name);
    printf("\n");
    printf("\n\tTasks:");

    list_foreach(Task, t, tasks)
    {
        task_dump(t);
    }

    atomic_end();
}

/* --- Current working directory -------------------------------------------- */

Path *task_cwd_resolve(Task *task, const char *buffer)
{
    Path *path = path_create(buffer);

    if (path_is_relative(path))
    {
        lock_acquire(task->cwd_lock);

        Path *combined = path_combine(task->cwd_path, path);
        path_destroy(path);
        path = combined;

        lock_release(task->cwd_lock);
    }

    path_normalize(path);

    return path;
}

error_t task_set_cwd(Task *task, const char *buffer)
{
    error_t result = ERR_SUCCESS;

    Path *path = task_cwd_resolve(task, buffer);
    FsNode *node = filesystem_find_and_ref(path);

    if (node == NULL)
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
        goto cleanup_and_return;
    }

    if (node->type != FSNODE_DIRECTORY)
    {
        result = ERR_NOT_A_DIRECTORY;
        goto cleanup_and_return;
    }

    lock_acquire(task->cwd_lock);

    path_destroy(task->cwd_path);
    task->cwd_path = path;
    path = NULL;

    lock_release(task->cwd_lock);

cleanup_and_return:
    if (node)
        fsnode_deref(node);

    if (path)
        path_destroy(path);

    return result;
}

void task_get_cwd(Task *task, char *buffer, uint size)
{
    lock_acquire(task->cwd_lock);

    path_to_cstring(task->cwd_path, buffer, size);

    lock_release(task->cwd_lock);
}

/* -------------------------------------------------------------------------- */
/*   SHARED MEMORY                                                            */
/* -------------------------------------------------------------------------- */

static int SHMID = 0;
static Lock shms_lock;
static List *shms;

void task_shared_memory_setup(void)
{
    shms = list_create();
    lock_init(shms_lock);
}

/* --- Shared phycical region ----------------------------------------------- */

void shm_physical_region_delete(shm_physical_region_t *task);
shm_physical_region_t *shm_physical_region(int pagecount)
{
    shm_physical_region_t *task = OBJECT(shm_physical_region);

    task->ID = SHMID++;
    task->paddr = physical_alloc(pagecount);
    task->pagecount = pagecount;

    if (task->paddr)
    {
        list_pushback(shms, task);

        return task;
    }
    else
    {
        object_release(task);

        return NULL;
    }
}

void shm_physical_region_delete(shm_physical_region_t *task)
{
    list_remove(shms, task);
    physical_free(task->paddr, task->pagecount);
}

shm_physical_region_t *task_physical_region_get_by_id(int id)
{
    if (id < SHMID)
        return NULL;

    list_foreach(shm_physical_region_t, shm, shms)
    {
        if (shm->ID == id)
            return shm;
    }

    return NULL;
}

/* --- Shared virtual region ------------------------------------------------ */
void shm_virtual_region_delete(shm_virtual_region_t *task);
shm_virtual_region_t *shm_virtual_region(Task *task, shm_physical_region_t *physr)
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

void shm_virtual_region_delete(shm_virtual_region_t *task)
{
    object_release(task->region);

    if (object_refcount(task->region) == 1)
    {
        // We were the last task to have a ref to this shm
        // The last ref is the shm list, so we can release that too

        object_release(task->region);
    }
}

shm_virtual_region_t *task_virtual_region_get_by_id(Task *task, int id)
{
    if (id < SHMID)
        return NULL;

    list_foreach(shm_virtual_region_t, shm, task->shms)
    {
        if (shm->region->ID == id)
            return shm;
    }

    return NULL;
}

/* --- User facing API ------------------------------------------------------ */

int task_shared_memory_alloc(Task *task, int pagecount)
{
    lock_acquire(shms_lock);

    shm_physical_region_t *physr = shm_physical_region(pagecount);

    if (physr == NULL)
    {
        return -ERR_CANNOT_ALLOCATE_MEMORY;
    }

    shm_virtual_region_t *virtr = shm_virtual_region(task, physr);

    if (virtr == NULL)
    {
        lock_release(shms_lock);
        return -ERR_CANNOT_ALLOCATE_MEMORY;
    }

    lock_release(shms_lock);
    return physr->ID;
}

int task_shared_memory_acquire(Task *task, int shm, uint *addr)
{
    lock_acquire(shms_lock);

    shm_virtual_region_t *virtr = task_virtual_region_get_by_id(task, shm);

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

    /* shm_virtual_region_t* */ virtr = shm_virtual_region(task, physr);

    if (virtr == NULL)
    {
        lock_release(shms_lock);
        return -ERR_CANNOT_ALLOCATE_MEMORY;
    }

    *addr = virtr->vaddr;
    lock_release(shms_lock);
    return -ERR_SUCCESS;
}

int task_shared_memory_release(Task *task, int shm)
{
    lock_acquire(shms_lock);

    shm_virtual_region_t *virtr = task_virtual_region_get_by_id(task, shm);

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

bool task_messaging_has_subscribe(Task *task, const char *channel)
{
    atomic_begin();

    list_foreach(const char, subscription, task->subscription)
    {
        if (strcmp(channel, subscription) == 0)
        {
            atomic_end();

            return true;
        }
    }

    atomic_end();

    return false;
}

int task_messaging_subscribe(Task *task, const char *channel)
{
    if (task_messaging_has_subscribe(task, channel))
    {
        return -ERR_SUCCESS;
    }
    else
    {
        atomic_begin();

        list_pushback(task->subscription, strdup(channel));

        atomic_end();
    }

    return -ERR_SUCCESS;
}

int task_messaging_unsubscribe(Task *task, const char *channel)
{
    if (task_messaging_has_subscribe(task, channel))
    {
        return -ERR_SUCCESS;
    }
    else
    {
        atomic_begin();

        list_foreach(char, subscription, task->subscription)
        {
            if (strcmp(channel, subscription) == 0)
            {
                list_remove(task->subscription, subscription);
                break;
            }
        }

        atomic_end();
    }

    return -ERR_SUCCESS;
}

/* --- Messages ------------------------------------------------------------- */

int task_messaging_send_internal(Task *task, Task *destination, message_t *event, message_type_t message_type)
{
    ASSERT_ATOMIC;

    if (!(list_count(destination->inbox) < 1024))
    {
        return -ERR_INBOX_FULL;
    }

    message_t *event_copy = __create(message_t);
    *event_copy = *event;

    event_copy->header.from = task->id;
    event_copy->header.to = destination->id;
    event_copy->header.type = message_type;

    list_pushback(destination->inbox, event_copy);

    if (destination->state == TASK_STATE_WAIT_MESSAGE)
    {
        task_set_state(destination, TASK_STATE_RUNNING);
    }

    return -ERR_SUCCESS;
}

int task_messaging_send(Task *task, message_t *event)
{
    atomic_begin();

    event->header.id = MID++;

    Task *destination = task_by_id(event->header.to);

    if (destination == NULL)
    {
        atomic_end();
        return -ERR_NO_SUCH_PROCESS;
    }

    int result = task_messaging_send_internal(task, destination, event, MESSAGE_TYPE_EVENT);

    atomic_end();

    return result;
}

int task_messaging_broadcast(Task *task, const char *channel, message_t *event)
{
    atomic_begin();

    event->header.id = MID++;

    list_foreach(Task, destination, task_all())
    {
        if (destination != task && task_messaging_has_subscribe(destination, channel))
        {
            task_messaging_send_internal(task, destination, event, MESSAGE_TYPE_EVENT);
        }
    }

    atomic_end();

    return -ERR_SUCCESS;
}

int task_messaging_request(Task *task, message_t *request, message_t *respond, int timeout)
{
    atomic_begin();
    {
        request->header.id = MID++;

        Task *destination = task_by_id(request->header.to);

        if (destination == NULL)
        {
            atomic_end();
            return -ERR_NO_SUCH_PROCESS;
        }

        task->wait.respond = (TaskWaitRespondInfo){
            .has_result = false,
            .result = {},
            .timeout = timeout,
        };

        task_messaging_send_internal(task, destination, request, MESSAGE_TYPE_REQUEST);

        task_set_state(task, TASK_STATE_WAIT_RESPOND);
    }
    atomic_end();

    sheduler_yield();

    if (task->wait.respond.has_result)
    {
        *respond = task->wait.respond.result;

        return -ERR_SUCCESS;
    }
    else
    {
        *respond = (message_t){0};
        return -ERR_REQUEST_TIMEOUT;
    }
}

int task_messaging_respond(Task *task, message_t *request, message_t *result)
{
    atomic_begin();

    Task *destination = task_by_id(request->header.from);
    if (destination == NULL || destination->state != TASK_STATE_WAIT_RESPOND)
    {
        atomic_end();
        return -ERR_NO_SUCH_PROCESS;
    }

    // FIXME: this is a lot of dot operator...
    destination->wait.respond.has_result = true;
    destination->wait.respond.result = *result;

    destination->wait.respond.result.header.from = task->id;
    destination->wait.respond.result.header.to = destination->id;

    task_set_state(destination, TASK_STATE_RUNNING);

    atomic_end();

    return -ERR_SUCCESS;
}

int task_messaging_receive(Task *task, message_t *message, bool wait)
{
    message_t *received = NULL;

    atomic_begin();

    if (list_pop(task->inbox, (void **)&received))
    {
        *message = *received;
    }

    if (received == NULL && wait)
    {
        task_set_state(task, TASK_STATE_WAIT_MESSAGE);

        atomic_end();

        sheduler_yield();

        atomic_begin();

        if (list_pop(task->inbox, (void **)&received))
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
    List *task_to_free = list_create();

    atomic_begin();
    // Get canceled tasks
    list_foreach(Task, canceled_tasks, task_by_state(TASK_STATE_CANCELED))
    {
        list_pushback(task_to_free, canceled_tasks);
    }

    atomic_end();

    // Cleanup all of those dead tasks.
    list_foreach(Task, task_to_cleanup, task_to_free)
    {
        task_destroy(task_to_cleanup);
    }

    list_destroy(task_to_free, LIST_KEEP_VALUES);
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

void timer_set_frequency(u16 hz)
{
    u32 divisor = 1193182 / hz;

    out8(0x43, 0x36);
    out8(0x40, divisor & 0xFF);
    out8(0x40, (divisor >> 8) & 0xFF);

    logger_info("Timer frequency is %dhz.", hz);
}

void sheduler_setup(Task *main_kernel_task)
{
    running = main_kernel_task;

    timer_set_frequency(1000);
}

/* --- Sheduling ------------------------------------------------------------ */

void wakeup_sleeping_tasks(void)
{
    if (!list_empty(task_by_state(TASK_STATE_WAIT_TIME)))
    {
        Task *task = NULL;

        do
        {
            if (list_peek(task_by_state(TASK_STATE_WAIT_TIME), (void **)&task))
            {
                if (task->wait.time.wakeuptick <= ticks)
                {
                    task_set_state(task, TASK_STATE_RUNNING);
                }
            }

        } while (task != NULL && task->state == TASK_STATE_RUNNING);
    }
}

void wakeup_blocked_task(void)
{
    if (list_any(task_by_state(TASK_STATE_BLOCKED)))
    {
        List *task_to_wakeup = list_create();

        list_foreach(Task, task, task_by_state(TASK_STATE_BLOCKED))
        {
            TaskBlocker *blocker = task->blocker;

            if (blocker->can_unblock(blocker, task))
            {
                if (blocker->unblock)
                {
                    blocker->unblock(blocker, task);
                }

                free(task->blocker);
                task->blocker = NULL;

                list_pushback(task_to_wakeup, task);
            }
        }

        list_foreach(Task, task, task_to_wakeup)
        {
            task_set_state(task, TASK_STATE_RUNNING);
        }

        list_destroy(task_to_wakeup, LIST_KEEP_VALUES);
    }
}

uintptr_t shedule(uintptr_t current_stack_pointer)
{
    sheduler_context_switch = true;

    // Save the old context
    running->stack_pointer = current_stack_pointer;
    platform_save_context(running);

    // Update the system ticks
    sheduler_record[ticks % SHEDULER_RECORD_COUNT] = running->id;
    ticks++;

    wakeup_sleeping_tasks();
    wakeup_blocked_task();

    // Get the next task
    if (!list_peek_and_pushback(task_by_state(TASK_STATE_RUNNING), (void **)&running))
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
    return running->stack_pointer;
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
    // FIXME: simple hack for system ticks.

    ticks--;
    asm("int $32");
}

/* --- Running task info -------------------------------------------------- */

Task *sheduler_running(void)
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
