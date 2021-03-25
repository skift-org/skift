#include <libsystem/Logger.h>

#include "kernel/tasking/Tasking.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Task.h"

static Iteration destroy_task_if_canceled(void *target, Task *task)
{
    __unused(target);

    if (task->state() == TASK_STATE_CANCELED)
    {
        task_destroy(task);
    }

    return Iteration::CONTINUE;
}

void garbage_collector()
{
    while (true)
    {
        task_sleep(scheduler_running(), 100);
        task_iterate(nullptr, destroy_task_if_canceled);
    }
}

void tasking_initialize()
{
    logger_info("Initializing tasking...");

    Task *idle_task = task_spawn(nullptr, "idle", system_hang, nullptr, TASK_NONE);
    task_go(idle_task);
    idle_task->state(TASK_STATE_HANG);

    scheduler_did_create_idle_task(idle_task);

    Task *kernel_task = task_spawn(nullptr, "system", nullptr, nullptr, TASK_NONE);
    task_go(kernel_task);

    scheduler_did_create_running_task(kernel_task);

    Task *garbage_task = task_spawn(nullptr, "garbage-collector", garbage_collector, nullptr, TASK_NONE);
    task_go(garbage_task);

    logger_info("Tasking initialized!");
}
