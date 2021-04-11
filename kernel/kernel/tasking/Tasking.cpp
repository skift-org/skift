#include <libsystem/Logger.h>

#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Finalizer.h"
#include "kernel/tasking/Task.h"

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

    Kernel::finalizer_initialize();

    logger_info("Tasking initialized!");
}
