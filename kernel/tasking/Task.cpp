#include <assert.h>
#include <string.h>

#include "archs/Architectures.h"
#include "archs/VirtualMemory.h"

#include "kernel/interrupts/Interupts.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Task-Memory.h"
#include "kernel/tasking/Task.h"

static int _task_ids = 0;
static List *_tasks;

TaskState Task::state()
{
    return _state;
}

void Task::state(TaskState state)
{
    scheduler_did_change_task_state(this, _state, state);
    _state = state;
}

void Task::interrupt()
{
    InterruptsRetainer retainer;

    _is_interrupted = true;

    if (_blocker)
    {
        _blocker->interrupt(*this, INTERRUPTED);
    }
}

Result Task::cancel(int exit_value)
{
    InterruptsRetainer retainer;

    if (_is_canceled)
    {
        return SUCCESS;
    }

    _is_canceled = true;
    this->exit_value = exit_value;

    if (_is_doing_syscall)
    {
        interrupt();
        return SUCCESS;
    }

    kill_me_if_you_dare();
    ASSERT_NOT_REACHED();
}

void Task::kill_me_if_you_dare()
{
    InterruptsRetainer retainer;

    if (_is_canceled)
    {
        if (_flags & TASK_WAITABLE)
        {
            state(TASK_STATE_CANCELING);
        }
        else
        {
            state(TASK_STATE_CANCELED);
        }

        if (this == scheduler_running())
        {
            scheduler_yield();
            ASSERT_NOT_REACHED();
        }
    }
}

Task *task_create(Task *parent, const char *name, TaskFlags flags)
{
    __unused(parent);

    ASSERT_INTERRUPTS_RETAINED();

    if (_tasks == nullptr)
    {
        _tasks = list_create();
    }

    Task *task = new Task{};

    task->id = _task_ids++;
    strlcpy(task->name, name, PROCESS_NAME_SIZE);
    task->_state = TASK_STATE_NONE;
    task->_flags = flags;

    if (task->_flags & TASK_USER)
    {
        task->address_space = arch_address_space_create();
    }
    else
    {
        task->address_space = arch_kernel_address_space();
    }

    if (parent)
        task->_domain = parent->_domain;

    // Setup shms
    task->memory_mapping = list_create();

    memory_alloc(task->address_space, PROCESS_STACK_SIZE, MEMORY_CLEAR, (uintptr_t *)&task->kernel_stack);
    task->kernel_stack_pointer = ((uintptr_t)task->kernel_stack + PROCESS_STACK_SIZE);

    if (task->_flags & TASK_USER)
    {
        void *parent_address_space = task_switch_address_space(scheduler_running(), task->address_space);
        task_memory_map(task, 0xff000000, PROCESS_STACK_SIZE, MEMORY_CLEAR | MEMORY_USER);
        task->user_stack_pointer = 0xff000000 + PROCESS_STACK_SIZE;
        task->user_stack = (void *)0xff000000;
        task_switch_address_space(scheduler_running(), parent_address_space);
    }

    arch_save_context(task);

    list_pushback(_tasks, task);

    return task;
}

Task *task_clone(Task *parent, uintptr_t sp, uintptr_t ip, TaskFlags flags)
{
    ASSERT_INTERRUPTS_RETAINED();

    if (_tasks == nullptr)
    {
        _tasks = list_create();
    }

    Task *task = new Task{};

    task->id = _task_ids++;
    task->_flags = flags;
    strlcpy(task->name, parent->name, PROCESS_NAME_SIZE);
    task->_state = TASK_STATE_NONE;

    task->address_space = arch_address_space_create();

    // Setup shms
    task->memory_mapping = list_create();

    if (parent)
        task->_domain = parent->_domain;

    // Setup fildes
    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        parent->handles().pass(task->handles(), i, i);
    }

    memory_alloc(task->address_space, PROCESS_STACK_SIZE, MEMORY_CLEAR, (uintptr_t *)&task->kernel_stack);
    task->kernel_stack_pointer = ((uintptr_t)task->kernel_stack + PROCESS_STACK_SIZE);

    list_foreach(MemoryMapping, mapping, parent->memory_mapping)
    {
        auto virtual_range = mapping->range();

        auto physical_range = mapping->object->range();

        void *buffer = malloc(physical_range.size());
        assert(buffer);
        assert(virtual_range.base());
        memcpy(buffer, (void *)virtual_range.base(), virtual_range.size());

        void *parent_address_space = task_switch_address_space(scheduler_running(), task->address_space);

        task_memory_map(task, virtual_range.base(), virtual_range.size(), MEMORY_USER);
        memcpy((void *)virtual_range.base(), buffer, virtual_range.size());

        task_switch_address_space(scheduler_running(), parent_address_space);

        free(buffer);
    }

    task->user_stack_pointer = sp;
    task->entry_point = (TaskEntryPoint)ip;

    list_pushback(_tasks, task);

    task_go(task);

    return task;
}

void task_destroy(Task *task)
{
    interrupts_retain();
    task->state(TASK_STATE_NONE);

    list_remove(_tasks, task);

    interrupts_release();

    MemoryMapping *mapping = nullptr;

    while ((mapping = (MemoryMapping *)list_peek(task->memory_mapping)))
    {
        task_memory_mapping_destroy(task, mapping);
    }

    list_destroy(task->memory_mapping);

    memory_free(task->address_space, MemoryRange{(uintptr_t)task->kernel_stack, PROCESS_STACK_SIZE});

    if (task->address_space != arch_kernel_address_space())
    {
        arch_address_space_destroy(task->address_space);
    }

    delete task;
}

void task_clear_userspace(Task *task)
{
    MemoryMapping *mapping = nullptr;

    while ((mapping = (MemoryMapping *)list_peek(task->memory_mapping)))
    {
        task_memory_mapping_destroy(task, mapping);
    }

    void *parent_address_space = task_switch_address_space(scheduler_running(), task->address_space);
    task_memory_map(task, 0xff000000, PROCESS_STACK_SIZE, MEMORY_CLEAR | MEMORY_USER);
    task->user_stack_pointer = 0xff000000 + PROCESS_STACK_SIZE;
    task->user_stack = (void *)0xff000000;
    task_switch_address_space(scheduler_running(), parent_address_space);
}

void task_iterate(void *target, TaskIterateCallback callback)
{
    InterruptsRetainer retainer;

    if (!_tasks)
    {
        return;
    }

    list_iterate(_tasks, target, (ListIterationCallback)callback);
}

Task *task_by_id(int id)
{
    list_foreach(Task, task, _tasks)
    {
        if (task->id == id)
        {
            return task;
        }
    }

    return nullptr;
}

int task_count()
{
    InterruptsRetainer retainer;

    if (!_tasks)
    {
        return 0;
    }

    return _tasks->count();
}

Task *task_spawn(Task *parent, const char *name, TaskEntryPoint entry, void *arg, TaskFlags flags)
{
    ASSERT_INTERRUPTS_RETAINED();

    Task *task = task_create(parent, name, flags);
    task_set_entry(task, entry);
    task_kernel_stack_push(task, &arg, sizeof(arg));

    return task;
}

void task_set_entry(Task *task, TaskEntryPoint entry)
{
    task->entry_point = entry;
}

uintptr_t task_kernel_stack_push(Task *task, const void *value, size_t size)
{
    task->kernel_stack_pointer -= size;
    memcpy((void *)task->kernel_stack_pointer, value, size);

    return task->kernel_stack_pointer;
}

uintptr_t task_user_stack_push(Task *task, const void *value, size_t size)
{
    task->user_stack_pointer -= size;
    memcpy((void *)task->user_stack_pointer, value, size);
    return task->user_stack_pointer;
}

uintptr_t task_user_stack_push_long(Task *task, long value)
{
    return task_user_stack_push(task, &value, sizeof(value));
}

uintptr_t task_user_stack_push_ptr(Task *task, void *ptr)
{
    return task_user_stack_push(task, &ptr, sizeof(ptr));
}

void task_go(Task *task)
{
    InterruptsRetainer retainer;

    arch_task_go(task);

    task->state(TASK_STATE_RUNNING);
}

Result task_sleep(Task *task, int timeout)
{
    BlockerTime blocker{};
    return task_block(task, blocker, timeout);
}

Result task_wait(int task_id, int *exit_value)
{
    InterruptsRetainer retainer;

    Task *task = task_by_id(task_id);

    if (!task)
    {
        return ERR_NO_SUCH_TASK;
    }

    if (!(task->_flags & TASK_USER))
    {
        return ERR_TASK_NOT_WAITABLE;
    }

    BlockerWait blocker{task, exit_value};
    return task_block(scheduler_running(), blocker, -1);
}

Result task_block(Task *task, Blocker &blocker, Timeout timeout)
{
    assert(!task->_blocker);

    interrupts_retain();
    if (task->_is_interrupted)
    {
        interrupts_release();
        return INTERRUPTED;
    }

    if (blocker.can_unblock(*task))
    {
        blocker.unblock(*task);
        interrupts_release();

        return blocker.result();
    }

    blocker.timeout(timeout == (Timeout)-1 ? -1 : system_get_tick() + timeout);

    task->_blocker = &blocker;
    task->state(TASK_STATE_BLOCKED);

    interrupts_release();
    scheduler_yield();

    task->_blocker = nullptr;

    return blocker.result();
}

void task_dump(Task *task)
{
    if (!task)
        return;

    InterruptsRetainer retainer;

    stream_format(out_stream, "\n\t - Task %d %s", task->id, task->name);
    stream_format(out_stream, "\n\t   State: %s", task_state_string(task->state()));
    stream_format(out_stream, "\n\t   Memory: ");

    list_foreach(MemoryMapping, mapping, task->memory_mapping)
    {
        auto virtual_range = mapping->range();
        stream_format(out_stream, "\n\t   - %08x - %08x (%08x)", virtual_range.base(), virtual_range.end(), virtual_range.size());
    }

    if (task->address_space == arch_kernel_address_space())
    {
        stream_format(out_stream, "\n\t   Address Space: %08x (kpdir)", task->address_space);
    }
    else
    {
        stream_format(out_stream, "\n\t   Address Space: %08x", task->address_space);
    }

    stream_format(out_stream, "\n");
}
