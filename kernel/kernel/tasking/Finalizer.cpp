#include "kernel/tasking/Finalizer.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/scheduling/Scheduler.h"

namespace Kernel
{

static List *_task_to_finalize = nullptr;

void finalize_task(Task *task)
{
    ASSERT_INTERRUPTS_RETAINED();

    assert(_task_to_finalize != nullptr);
    list_pushback(_task_to_finalize, task);
}

Task *finalizer_pop_task()
{
    InterruptsRetainer retainer;
    Task *task = nullptr;
    list_pop(_task_to_finalize, (void **)&task);
    return task;
}

void finalizer_task()
{
    assert(_task_to_finalize != nullptr);

    while (true)
    {
        task_sleep(scheduler_running(), 100);

        Task *task_to_destroy = nullptr;
        while ((task_to_destroy = finalizer_pop_task()))
        {
            task_destroy(task_to_destroy);
        }
    }
}

void finalizer_initialize()
{
    _task_to_finalize = list_create();

    Task *finalizer = task_spawn(nullptr, "finalizer", finalizer_task, nullptr, TASK_NONE);
    task_go(finalizer);
}

} // namespace Kernel