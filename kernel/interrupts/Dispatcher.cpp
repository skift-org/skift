
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/thread/Atomic.h>
#include <libutils/RingBuffer.h>

#include "kernel/interrupts/Dispatcher.h"
#include "kernel/scheduling/Blocker.h"
#include "kernel/scheduling/Scheduler.h"

static RingBuffer *_interupts_to_dispatch = nullptr;
static DispatcherInteruptHandler _interupts_to_handlers[255] = {};

void dispatcher_initialize()
{
    _interupts_to_dispatch = new RingBuffer(1024);

    Task *interrupts_dispatcher_task = task_spawn(nullptr, "InterruptsDispatcher", dispatcher_service, nullptr, false);
    task_go(interrupts_dispatcher_task);
}

void dispatcher_dispatch(int interrupt)
{
    if (_interupts_to_handlers[interrupt])
    {
        _interupts_to_dispatch->put(interrupt);
    }
}

static bool dispatcher_has_interupt()
{
    AtomicHolder holder;

    return !_interupts_to_dispatch->empty();
}

static int dispatcher_get_interupt()
{
    AtomicHolder holder;
    return _interupts_to_dispatch->get();
}

class BlockerDispatcher : public Blocker
{
private:
public:
    BlockerDispatcher() {}

    bool can_unblock(struct Task *task)
    {
        __unused(task);

        return dispatcher_has_interupt();
    }
};

void dispatcher_service()
{
    while (true)
    {
        task_block(scheduler_running(), new BlockerDispatcher(), -1);

        while (dispatcher_has_interupt())
        {
            int interrupt = dispatcher_get_interupt();

            if (_interupts_to_handlers[interrupt])
            {
                _interupts_to_handlers[interrupt]();
            }
            else
            {
                logger_warn("No handler for interrupt %d!", interrupt);
            }
        }
    }
}

void dispatcher_register_handler(int interrupt, DispatcherInteruptHandler handler)
{
    assert(!_interupts_to_handlers[interrupt]);
    _interupts_to_handlers[interrupt] = handler;
}

void dispatcher_unregister_handler(DispatcherInteruptHandler handler)
{
    for (int i = 0; i < 255; i++)
    {
        if (_interupts_to_handlers[i] == handler)
        {
            _interupts_to_handlers[i] = nullptr;
        }
    }
}
