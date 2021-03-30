
#include "archs/Architectures.h"
#include "archs/VirtualMemory.h"

#include "kernel/interrupts/Interupts.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"

static bool scheduler_context_switch = false;
static int scheduler_record[SCHEDULER_RECORD_COUNT] = {};

static Task *running = nullptr;
static Task *idle = nullptr;

static List *blocked_tasks;
static List *running_tasks;

void scheduler_initialize()
{
    blocked_tasks = list_create();
    running_tasks = list_create();
}

void scheduler_did_create_idle_task(Task *task)
{
    idle = task;
}

void scheduler_did_create_running_task(Task *task)
{
    running = task;
}

void scheduler_did_change_task_state(Task *task, TaskState oldstate, TaskState newstate)
{
    ASSERT_INTERRUPTS_RETAINED();

    if (oldstate != newstate)
    {
        if (oldstate == TASK_STATE_RUNNING)
        {
            list_remove(running_tasks, task);
        }

        if (oldstate == TASK_STATE_BLOCKED)
        {
            list_remove(blocked_tasks, task);
        }

        if (newstate == TASK_STATE_BLOCKED)
        {
            list_push(blocked_tasks, task);
        }

        if (newstate == TASK_STATE_RUNNING)
        {
            list_push(running_tasks, task);
        }
    }
}

bool scheduler_is_context_switch()
{
    return scheduler_context_switch;
}

Task *scheduler_running()
{
    return running;
}

int scheduler_running_id()
{
    if (running == nullptr)
    {
        return -1;
    }

    return running->id;
}

void scheduler_yield()
{
    ASSERT_INTERRUPTS_NOT_RETAINED();
    arch_yield();
    ASSERT_INTERRUPTS_NOT_RETAINED();
}

int scheduler_get_usage(int task_id)
{
    InterruptsRetainer retainer;

    int count = 0;

    for (int i = 0; i < SCHEDULER_RECORD_COUNT; i++)
    {
        if (scheduler_record[i] == task_id)
        {
            count++;
        }
    }

    return (count * 100) / SCHEDULER_RECORD_COUNT;
}

static Iteration wakeup_task_if_unblocked(void *, Task *task)
{
    task->try_unblock();

    return Iteration::CONTINUE;
}

uintptr_t schedule(uintptr_t current_stack_pointer)
{
    scheduler_context_switch = true;

    running->kernel_stack_pointer = current_stack_pointer;
    arch_save_context(running);

    scheduler_record[system_get_tick() % SCHEDULER_RECORD_COUNT] = running->id;

    list_iterate(blocked_tasks, nullptr, (ListIterationCallback)wakeup_task_if_unblocked);

    // Get the next task
    if (!list_peek_and_pushback(running_tasks, (void **)&running))
    {
        // Or the idle task if there are no running tasks.
        running = idle;
    }

    arch_address_space_switch(running->address_space);
    arch_load_context(running);

    scheduler_context_switch = false;

    return running->kernel_stack_pointer;
}
