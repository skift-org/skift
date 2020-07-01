#include <libsystem/Assert.h>
#include <libsystem/Atomic.h>
#include <libsystem/CString.h>

#include "arch/Arch.h"
#include "kernel/tasking.h"
#include "kernel/tasking/Handles.h"

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