#include <libsystem/Assert.h>
#include <libsystem/Atomic.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>

#include "arch/Arch.h"
#include "arch/x86/Interrupts.h" /* XXX */
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Task-Handles.h"
#include "kernel/tasking/Task-Memory.h"
#include "kernel/tasking/Task.h"

static int _task_ids = 0;
static List *_tasks;

Task *task_create(Task *parent, const char *name, bool user)
{
    ASSERT_ATOMIC;

    if (_tasks == NULL)
    {
        _tasks = list_create();
    }

    Task *task = __create(Task);

    task->id = _task_ids++;
    strlcpy(task->name, name, PROCESS_NAME_SIZE);
    task->state = TASK_STATE_NONE;

    // Setup memory space
    if (user)
    {
        task->pdir = memory_pdir_create();
    }
    else
    {
        task->pdir = memory_kpdir();
    }

    // Setup shms
    task->memory_mapping = list_create();

    // Setup current working directory.
    lock_init(task->directory_lock);

    if (parent != NULL)
    {
        task->directory = path_clone(parent->directory);
    }
    else
    {
        Path *path = path_create("/");
        task->directory = path;
        assert(task->directory);
    }

    // Setup fildes
    lock_init(task->handles_lock);
    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        task->handles[i] = NULL;
    }

    memory_alloc(task->pdir, PROCESS_STACK_SIZE, MEMORY_CLEAR, (uintptr_t *)&task->stack);
    task->stack_pointer = ((uintptr_t)task->stack + PROCESS_STACK_SIZE - 1);

    arch_save_context(task);

    list_pushback(_tasks, task);

    return task;
}

void task_destroy(Task *task)
{
    atomic_begin();

    if (task->state != TASK_STATE_NONE)
        task_set_state(task, TASK_STATE_NONE);

    list_remove(_tasks, task);
    atomic_end();

    list_foreach(MemoryMapping, memory_mapping, task->memory_mapping)
    {
        task_memory_mapping_destroy(task, memory_mapping);
    }

    list_destroy(task->memory_mapping);

    task_fshandle_close_all(task);

    lock_acquire(task->directory_lock);
    path_destroy(task->directory);

    memory_free(task->pdir, (MemoryRange){(uintptr_t)task->stack, PROCESS_STACK_SIZE});

    if (task->pdir != memory_kpdir())
    {
        memory_pdir_destroy(task->pdir);
    }

    free(task);
}

void task_iterate(void *target, TaskIterateCallback callback)
{
    atomic_begin();
    if (!_tasks)
    {
        atomic_end();
        return;
    }

    list_iterate(_tasks, target, (ListIterationCallback)callback);
    atomic_end();
}

Task *task_by_id(int id)
{
    list_foreach(Task, task, _tasks)
    {
        if (task->id == id)
            return task;
    }

    return NULL;
}

int task_count(void)
{
    atomic_begin();
    if (!_tasks)
    {
        atomic_end();
        return 0;
    }

    int result = list_count(_tasks);
    atomic_end();

    return result;
}

Task *task_spawn(Task *parent, const char *name, TaskEntry entry, void *arg, bool user)
{
    ASSERT_ATOMIC;

    Task *task = task_create(parent, name, user);

    task_set_entry(task, entry, user);
    task_stack_push(task, &arg, sizeof(arg));

    return task;
}

Task *task_spawn_with_argv(Task *parent, const char *name, TaskEntry entry, const char **argv, bool user)
{
    atomic_begin();

    Task *task = task_create(parent, name, user);

    task_set_entry(task, entry, true);

    uintptr_t argv_list[PROCESS_ARG_COUNT] = {};

    int argc;
    for (argc = 0; argv[argc] && argc < PROCESS_ARG_COUNT; argc++)
    {
        argv_list[argc] = task_stack_push(task, argv[argc], strlen(argv[argc]) + 1);
    }

    uintptr_t argv_list_ref = task_stack_push(task, &argv_list, sizeof(argv_list));

    task_stack_push(task, &argv_list_ref, sizeof(argv_list_ref));
    task_stack_push(task, &argc, sizeof(argc));

    atomic_end();

    return task;
}

void task_set_state(Task *task, TaskState state)
{
    ASSERT_ATOMIC;

    scheduler_did_change_task_state(task, task->state, state);
    task->state = state;
}

void task_set_entry(Task *task, TaskEntry entry, bool user)
{
    task->entry = entry;
    task->user = user;
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

    // TODO: All tasks are still running has ring0 :/
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

Result task_sleep(Task *task, int timeout)
{
    task_block(task, blocker_time_create(system_get_tick() + timeout), -1);

    return TIMEOUT;
}

Result task_wait(int task_id, int *exit_value)
{
    atomic_begin();
    Task *task = task_by_id(task_id);

    if (!task)
    {
        atomic_end();

        return ERR_NO_SUCH_TASK;
    }

    atomic_end();

    task_block(
        scheduler_running(),
        blocker_wait_create(task, exit_value), -1);

    return SUCCESS;
}

BlockerResult task_block(Task *task, Blocker *blocker, Timeout timeout)
{
    assert(!task->blocker);

    atomic_begin();

    task->blocker = blocker;
    if (blocker->can_unblock(blocker, task))
    {

        if (blocker->on_unblock)
        {
            blocker->on_unblock(blocker, task);
        }

        atomic_end();

        task->blocker = NULL;
        free(blocker);

        return BLOCKER_UNBLOCKED;
    }

    if (timeout == (Timeout)-1)
    {
        blocker->timeout = (Timeout)-1;
    }
    else
    {
        blocker->timeout = system_get_tick() + timeout;
    }

    task_set_state(task, TASK_STATE_BLOCKED);
    atomic_end();

    scheduler_yield();

    BlockerResult result = blocker->result;

    task->blocker = NULL;
    free(blocker);

    return result;
}

Result task_cancel(Task *task, int exit_value)
{
    assert(task);

    atomic_begin();

    task->exit_value = exit_value;
    task_set_state(task, TASK_STATE_CANCELED);

    atomic_end();

    return SUCCESS;
}

void task_exit(int exit_value)
{
    task_cancel(scheduler_running(), exit_value);

    scheduler_yield();

    ASSERT_NOT_REACHED();
}

void task_dump(Task *task)
{
    atomic_begin();
    printf("\n\t - Task %d %s", task->id, task->name);
    printf("\n\t   State: %s", task_state_string(task->state));
    printf("\n\t   User memory: ");
    memory_pdir_dump(task->pdir, false);

    if (task->pdir == memory_kpdir())
    {
        printf("\n\t   Page directory: %08x (kpdir)", task->pdir);
    }
    else
    {
        printf("\n\t   Page directory: %08x", task->pdir);
    }

    printf("\n");
    atomic_end();
}
