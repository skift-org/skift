/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/RingBuffer.h>
#include <libsystem/assert.h>
#include <libsystem/atomic.h>

#include "kernel/interrupts/Dispatcher.h"
#include "kernel/sheduling/TaskBlocker.h"
#include "kernel/tasking.h"

static RingBuffer *_interupts_to_dispatch = NULL;
static DispatcherInteruptHandler _interupts_to_handlers[255] = {};

void dispatcher_initialize(void)
{
    _interupts_to_dispatch = ringbuffer_create(1024);

    Task *interrupts_dispatcher_task = task_spawn(NULL, "InterruptsDispatcher", dispatcher_service, NULL, false);
    task_go(interrupts_dispatcher_task);
}

void dispatcher_dispatch(int interrupt)
{
    ringbuffer_putc(_interupts_to_dispatch, interrupt);
}

bool dispatcher_can_unblock(TaskBlocker *blocker, Task *task)
{
    __unused(blocker);
    __unused(task);

    return !ringbuffer_is_empty(_interupts_to_dispatch);
}

void dispatcher_service(void)
{
    while (true)
    {
        atomic_begin();

        bool should_block = ringbuffer_is_empty(_interupts_to_dispatch);

        atomic_end();

        if (should_block)
        {
            TaskBlocker *blocker = __create(TaskBlocker);
            blocker->can_unblock = (TaskBlockerCanUnblockCallback)dispatcher_can_unblock;
            task_block(sheduler_running(), blocker, 0);
        }

        atomic_begin();

        int interrupt = ringbuffer_getc(_interupts_to_dispatch);

        atomic_end();

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
            _interupts_to_handlers[i] = NULL;
        }
    }
}
