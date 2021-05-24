#include "system/tasking/Finalizer.h"
#include "system/interrupts/Interupts.h"
#include "system/scheduling/Scheduler.h"

namespace Kernel
{

static List<Task *> *_task_to_finalize = nullptr;

void finalize_task(Task *task)
{
    ASSERT_INTERRUPTS_RETAINED();

    assert(_task_to_finalize != nullptr);
    _task_to_finalize->push_back(task);
}

Task *finalizer_pop_task()
{
    InterruptsRetainer retainer;
    if (_task_to_finalize->any())
    {
        return _task_to_finalize->pop();
    }
    else
    {
        return nullptr;
    }
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
    _task_to_finalize = new List<Task *>();

    Task *finalizer = task_spawn(nullptr, "finalizer", finalizer_task, nullptr, TASK_NONE);
    task_go(finalizer);
}

} // namespace Kernel