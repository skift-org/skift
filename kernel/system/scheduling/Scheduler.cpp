#include <libutils/List.h>

#include "archs/Arch.h"

#include "system/interrupts/Interupts.h"
#include "system/scheduling/Scheduler.h"
#include "system/system/System.h"

static bool scheduler_context_switch = false;
static int scheduler_record[SCHEDULER_RECORD_COUNT] = {};

static Task *running = nullptr;
static Task *idle = nullptr;

static Utils::List<Task *> *blocked_tasks;
static Utils::List<Task *> *running_tasks;

void scheduler_initialize()
{
    blocked_tasks = new Utils::List<Task *>();
    running_tasks = new Utils::List<Task *>();
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
            running_tasks->remove(task);
        }

        if (oldstate == TASK_STATE_BLOCKED)
        {
            blocked_tasks->remove(task);
        }

        if (newstate == TASK_STATE_BLOCKED)
        {
            blocked_tasks->push_back(task);
        }

        if (newstate == TASK_STATE_RUNNING)
        {
            // Maybe we should push the task to the start of the list if it's a low latency tasks.
            running_tasks->push_back(task);
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
    Arch::yield();
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

uintptr_t schedule(uintptr_t current_stack_pointer)
{
    scheduler_context_switch = true;

    running->kernel_stack_pointer = current_stack_pointer;
    Arch::save_context(running);

    scheduler_record[system_get_tick() % SCHEDULER_RECORD_COUNT] = running->id;

    blocked_tasks->foreach([](auto &task) {
        task->try_unblock();
        return Iteration::CONTINUE;
    });

    running = running_tasks->requeue().unwrap_or(idle);

    Arch::address_space_switch(running->address_space);
    Arch::load_context(running);

    scheduler_context_switch = false;

    return running->kernel_stack_pointer;
}
