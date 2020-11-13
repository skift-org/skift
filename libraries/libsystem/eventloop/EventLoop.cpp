
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Invoker.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/system/System.h>
#include <libsystem/utils/List.h>
#include <libutils/Vector.h>

static TimeStamp _eventloop_timer_last_fire = 0;

static Vector<Timer *> _eventloop_timers;
static List *_eventloop_notifiers = nullptr;
static Vector<Invoker *> _eventloop_invoker;

static size_t _eventloop_handles_count;
static Handle *_eventloop_handles[PROCESS_HANDLE_COUNT];
static PollEvent _eventloop_events[PROCESS_HANDLE_COUNT];

static bool _eventloop_is_running = false;
static bool _eventloop_is_initialize = false;
static int _eventloop_exit_value = PROCESS_SUCCESS;

static bool _nested_eventloop_is_running = false;
static int _nested_eventloop_exit_value = 0;

void eventloop_initialize()
{
    assert(!_eventloop_is_initialize);

    _eventloop_timer_last_fire = system_get_ticks();

    _eventloop_notifiers = list_create();

    _eventloop_is_initialize = true;
}

void eventloop_uninitialize()
{
    assert(_eventloop_is_initialize);

    list_destroy(_eventloop_notifiers);

    _eventloop_is_initialize = false;
}

int eventloop_run()
{
    assert(_eventloop_is_initialize);
    assert(!_eventloop_is_running);

    _eventloop_is_running = true;

    while (_eventloop_is_running)
    {
        eventloop_pump(false);
    }

    eventloop_uninitialize();

    return _eventloop_exit_value;
}

int eventloop_run_nested()
{
    assert(_eventloop_is_initialize);
    assert(_eventloop_is_running);
    assert(!_nested_eventloop_is_running);

    _nested_eventloop_is_running = true;

    while (_nested_eventloop_is_running)
    {
        eventloop_pump(false);
    }

    return _nested_eventloop_exit_value;
}

static Timeout eventloop_get_timeout()
{
    Timeout timeout = UINT32_MAX;

    TimeStamp current_tick = system_get_ticks();

    _eventloop_timers.foreach ([&](auto timer) {
        if (timer->running() && timer->interval() != 0)
        {
            if (timer->scheduled() < current_tick)
            {
                timeout = 0;
            }
            else
            {
                Timeout remaining = timer->scheduled() - current_tick;

                if (remaining <= timeout)
                {
                    timeout = remaining;
                }
            }
        }

        return Iteration::CONTINUE;
    });

    return timeout;
}

void eventloop_update_timers()
{
    assert(_eventloop_is_initialize);

    TimeStamp current_fire = system_get_ticks();

    auto timers_list_copy = _eventloop_timers;

    timers_list_copy.foreach ([&](auto timer) {
        if (timer->running() && timer->scheduled() <= current_fire)
        {
            timer->trigger();
            timer->schedule(current_fire + timer->interval());
        }

        return Iteration::CONTINUE;
    });

    _eventloop_timer_last_fire = current_fire;
}

void eventloop_pump(bool pool)
{
    assert(_eventloop_is_initialize);

    Timeout timeout = eventloop_get_timeout();

    if (pool)
    {
        timeout = 0;
    }

    eventloop_update_timers();

    Handle *selected = nullptr;
    PollEvent selected_events = 0;

    Result result = handle_poll(
        &_eventloop_handles[0],
        &_eventloop_events[0],
        _eventloop_handles_count,
        &selected,
        &selected_events,
        timeout);

    if (result_is_error(result))
    {
        logger_error("Failed to select : %s", result_to_string(result));
        eventloop_exit(PROCESS_FAILURE);
    }

    eventloop_update_timers();

    list_foreach(Notifier, notifier, _eventloop_notifiers)
    {
        if (notifier->handle == selected)
        {
            notifier->callback(notifier->target, notifier->handle, selected_events);
        }
    }

    _eventloop_invoker.foreach ([](Invoker *invoker) {
        if (invoker->should_be_invoke_later())
        {
            invoker->invoke();
        }

        return Iteration::CONTINUE;
    });
}

void eventloop_exit(int exit_value)
{
    assert(_eventloop_is_initialize);

    _eventloop_is_running = false;
    _eventloop_exit_value = exit_value;
}

void eventloop_exit_nested(int exit_value)
{
    assert(_eventloop_is_initialize);
    assert(_nested_eventloop_is_running);

    _nested_eventloop_is_running = false;
    _nested_eventloop_exit_value = exit_value;
}

void eventloop_update_notifier()
{
    _eventloop_handles_count = 0;

    list_foreach(Notifier, notifier, _eventloop_notifiers)
    {
        _eventloop_handles[_eventloop_handles_count] = notifier->handle;
        _eventloop_events[_eventloop_handles_count] = notifier->events;

        _eventloop_handles_count++;
    }
}
void eventloop_register_notifier(Notifier *notifier)
{
    assert(_eventloop_is_initialize);

    list_pushback(_eventloop_notifiers, notifier);

    eventloop_update_notifier();
}

void eventloop_unregister_notifier(Notifier *notifier)
{
    assert(_eventloop_is_initialize);

    list_remove(_eventloop_notifiers, notifier);

    eventloop_update_notifier();
}

void eventloop_register_timer(struct Timer *timer)
{
    assert(_eventloop_is_initialize);

    _eventloop_timers.push_back(timer);
}

void eventloop_unregister_timer(struct Timer *timer)
{
    assert(_eventloop_is_initialize);
    _eventloop_timers.remove_value(timer);
}

void eventloop_register_invoker(struct Invoker *invoker)
{
    assert(_eventloop_is_initialize);
    _eventloop_invoker.push_back(invoker);
}

void eventloop_unregister_invoker(struct Invoker *invoker)
{
    assert(_eventloop_is_initialize);
    _eventloop_invoker.remove_value(invoker);
}
