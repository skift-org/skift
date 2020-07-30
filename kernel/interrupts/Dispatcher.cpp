
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/thread/Atomic.h>
#include <libsystem/utils/RingBuffer.h>

#include "kernel/interrupts/Dispatcher.h"
#include "kernel/scheduling/Blocker.h"
#include "kernel/scheduling/Scheduler.h"

static RingBuffer *_interupts_to_dispatch = nullptr;
static DispatcherInteruptHandler _interupts_to_handlers[255] = {};

void dispatcher_initialize()
{
    _interupts_to_dispatch = ringbuffer_create(1024);

    Task *interrupts_dispatcher_task = task_spawn(nullptr, "InterruptsDispatcher", dispatcher_service, nullptr, false);
    task_go(interrupts_dispatcher_task);
}

void dispatcher_dispatch(int interrupt)
{
    if (_interupts_to_handlers[interrupt])
    {
        ringbuffer_putc(_interupts_to_dispatch, interrupt);
    }
}

static bool dispatcher_has_interupt()
{
    atomic_begin();
    bool result = !ringbuffer_is_empty(_interupts_to_dispatch);
    atomic_end();

    return result;
}

static int dispatcher_get_interupt()
{
    atomic_begin();
    int interrupt = ringbuffer_getc(_interupts_to_dispatch);
    atomic_end();

    return interrupt;
}

static bool dispatcher_can_unblock(Blocker *blocker, Task *task)
{
    __unused(blocker);
    __unused(task);

    return dispatcher_has_interupt();
}

void dispatcher_service()
{
    while (true)
    {
        Blocker *blocker = __create(Blocker);
        blocker->can_unblock = (BlockerCanUnblockCallback)dispatcher_can_unblock;
        task_block(scheduler_running(), blocker, -1);

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
