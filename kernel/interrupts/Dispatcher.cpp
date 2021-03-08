#include <assert.h>
#include <libsystem/Logger.h>
#include <libutils/RingBuffer.h>

#include "kernel/devices/Devices.h"
#include "kernel/interrupts/Dispatcher.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/scheduling/Blocker.h"
#include "kernel/scheduling/Scheduler.h"

static bool _pending_interrupts[256] = {};

void dispatcher_initialize()
{
    Task *interrupts_dispatcher_task = task_spawn(nullptr, "interrupts-dispatcher", dispatcher_service, nullptr, false);
    task_go(interrupts_dispatcher_task);
}

void dispatcher_dispatch(int interrupt)
{
    _pending_interrupts[interrupt] = true;
    devices_acknowledge_interrupt(interrupt);
}

static bool dispatcher_has_interrupt()
{
    InterruptsRetainer retainer;

    int result = 0;

    for (size_t i = 0; i < 256; i++)
    {
        if (_pending_interrupts[i])
        {
            result++;
        }
    }

    return result > 0;
}

static void dispatcher_snapshot(bool *destination)
{
    InterruptsRetainer retainer;
    memcpy(destination, _pending_interrupts, sizeof(_pending_interrupts));
    memset(_pending_interrupts, 0, sizeof(_pending_interrupts));
}

class BlockerDispatcher : public Blocker
{
public:
    bool can_unblock(Task &) override
    {
        return dispatcher_has_interrupt();
    }
};

void dispatcher_service()
{
    while (true)
    {
        BlockerDispatcher blocker{};
        task_block(scheduler_running(), blocker, -1);

        while (dispatcher_has_interrupt())
        {
            bool snapshot[256];
            dispatcher_snapshot(snapshot);

            for (size_t i = 0; i < 256; i++)
            {
                if (snapshot[i])
                {
                    devices_handle_interrupt(i);
                }
            }
        }
    }
}
