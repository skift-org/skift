
#include <libfile/elf.h>
#include <libmath/math.h>
#include <libsystem/Assert.h>
#include <libsystem/Atomic.h>
#include <libsystem/CString.h>
#include <libsystem/Debug.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>

#include "arch/Arch.h"
#include "arch/x86/Interrupts.h" /* XXX */

#include "kernel/memory/Physical.h"
#include "kernel/memory/Virtual.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking.h"
#include "kernel/tasking/Handles.h"

/* -------------------------------------------------------------------------- */
/*   TASKING                                                                  */
/* -------------------------------------------------------------------------- */

void tasking_initialize(void)
{
    Task *idle_task = task_spawn(NULL, "Idle", system_hang, NULL, false);
    task_go(idle_task);
    task_set_state(idle_task, TASK_STATE_HANG);

    scheduler_did_create_idle_task(idle_task);

    Task *kernel_task = task_spawn(NULL, "System", NULL, NULL, false);
    task_go(kernel_task);

    scheduler_did_create_running_task(kernel_task);

    Task *garbage_task = task_spawn(NULL, "GarbageCollector", garbage_collector, NULL, false);
    task_go(garbage_task);
}

/* -------------------------------------------------------------------------- */
/*   TASKS                                                                    */
/* -------------------------------------------------------------------------- */

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

/* --- Tasks methodes ------------------------------------------------------- */

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

/* --- Task wait state ------------------------------------------------------ */

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

/* --- Task stopping and canceling ------------------------------------------ */

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

/* --- Task Memory management ----------------------------------------------- */

PageDirectory *task_switch_pdir(Task *task, PageDirectory *pdir)
{
    PageDirectory *oldpdir = task->pdir;

    task->pdir = pdir;

    memory_pdir_switch(pdir);

    return oldpdir;
}

Result task_memory_map(Task *task, MemoryRange range)
{
    return memory_map(task->pdir, range, MEMORY_USER | MEMORY_CLEAR);
}

Result task_memory_alloc(Task *task, size_t size, uintptr_t *out_address)
{
    return memory_alloc(task->pdir, size, MEMORY_USER | MEMORY_CLEAR, out_address);
}

Result task_memory_free(Task *task, MemoryRange range)
{
    return memory_free(task->pdir, range);
}

/* --- Task dump ------------------------------------------------------------ */

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

/* --- Current working directory -------------------------------------------- */

Path *task_resolve_directory(Task *task, const char *buffer)
{
    Path *path = path_create(buffer);

    if (path_is_relative(path))
    {
        lock_acquire(task->directory_lock);

        Path *combined = path_combine(task->directory, path);
        path_destroy(path);
        path = combined;

        lock_release(task->directory_lock);
    }

    path_normalize(path);

    return path;
}

Result task_set_directory(Task *task, const char *buffer)
{
    Result result = SUCCESS;

    Path *path = task_resolve_directory(task, buffer);
    FsNode *node = filesystem_find_and_ref(path);

    if (node == NULL)
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
        goto cleanup_and_return;
    }

    if (node->type != FILE_TYPE_DIRECTORY)
    {
        result = ERR_NOT_A_DIRECTORY;
        goto cleanup_and_return;
    }

    lock_acquire(task->directory_lock);

    path_destroy(task->directory);
    task->directory = path;
    path = NULL;

    lock_release(task->directory_lock);

cleanup_and_return:
    if (node)
        fsnode_deref(node);

    if (path)
        path_destroy(path);

    return result;
}

Result task_get_directory(Task *task, char *buffer, uint size)
{
    lock_acquire(task->directory_lock);

    path_to_cstring(task->directory, buffer, size);

    lock_release(task->directory_lock);

    return SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*   SHARED MEMORY                                                            */
/* -------------------------------------------------------------------------- */

/* --- Memory Mapping ------------------------------------------------------- */

MemoryMapping *task_memory_mapping_create(Task *task, MemoryObject *memory_object)
{
    MemoryMapping *memory_mapping = __create(MemoryMapping);

    memory_mapping->object = memory_object_ref(memory_object);
    memory_mapping->address = virtual_alloc(task->pdir, (MemoryRange){memory_object->address, memory_object->size}, MEMORY_USER).base;
    memory_mapping->size = memory_object->size;

    list_pushback(task->memory_mapping, memory_mapping);

    return memory_mapping;
}

void task_memory_mapping_destroy(Task *task, MemoryMapping *memory_mapping)
{
    virtual_free(task->pdir, (MemoryRange){memory_mapping->address, memory_mapping->size});
    memory_object_deref(memory_mapping->object);

    list_remove(task->memory_mapping, memory_mapping);
    free(memory_mapping);
}

MemoryMapping *task_memory_mapping_by_address(Task *task, uintptr_t address)
{
    list_foreach(MemoryMapping, memory_mapping, task->memory_mapping)
    {
        if (memory_mapping->address == address)
        {
            return memory_mapping;
        }
    }

    return NULL;
}

/* --- User facing API ------------------------------------------------------ */

Result task_shared_memory_alloc(Task *task, size_t size, uintptr_t *out_address)
{
    MemoryObject *memory_object = memory_object_create(size);

    MemoryMapping *memory_mapping = task_memory_mapping_create(task, memory_object);

    memory_object_deref(memory_object);

    *out_address = memory_mapping->address;

    return SUCCESS;
}

Result task_shared_memory_free(Task *task, uintptr_t address)
{
    MemoryMapping *memory_mapping = task_memory_mapping_by_address(task, address);

    if (!memory_mapping)
    {
        return ERR_BAD_ADDRESS;
    }

    task_memory_mapping_destroy(task, memory_mapping);

    return SUCCESS;
}

Result task_shared_memory_include(Task *task, int handle, uintptr_t *out_address, size_t *out_size)
{
    MemoryObject *memory_object = memory_object_by_id(handle);

    if (!memory_object)
    {
        return ERR_BAD_ADDRESS;
    }

    MemoryMapping *memory_mapping = task_memory_mapping_create(task, memory_object);

    memory_object_deref(memory_object);

    *out_address = memory_mapping->address;
    *out_size = memory_mapping->size;

    return SUCCESS;
}

Result task_shared_memory_get_handle(Task *task, uintptr_t address, int *out_handle)
{
    MemoryMapping *memory_mapping = task_memory_mapping_by_address(task, address);

    if (!memory_mapping)
    {
        return ERR_BAD_ADDRESS;
    }

    *out_handle = memory_mapping->object->id;
    return SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*   GARBAGE COLLECTOR                                                        */
/* -------------------------------------------------------------------------- */

static IterationDecision destroy_task_if_canceled(void *target, Task *task)
{
    __unused(target);

    if (task->state == TASK_STATE_CANCELED)
    {
        task_destroy(task);
    }

    return ITERATION_CONTINUE;
}

void garbage_collector(void)
{
    while (true)
    {
        task_sleep(scheduler_running(), 100);
        task_iterate(NULL, destroy_task_if_canceled);
    }
}
