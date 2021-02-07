
#include <assert.h>
#include <libsystem/Logger.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Invoker.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/system/System.h>
#include <libsystem/utils/List.h>
#include <libutils/Vector.h>

namespace EventLoop
{

/* --- Notifiers ------------------------------------------------------------ */

static size_t _handles_count;
static Handle *_handles[PROCESS_HANDLE_COUNT];
static PollEvent _events[PROCESS_HANDLE_COUNT];

static Vector<Notifier *> _notifiers;

void update_notifier()
{
    for (size_t i = 0; i < _notifiers.count(); i++)
    {
        _handles[i] = _notifiers[i]->handle();
        _events[i] = _notifiers[i]->events();
    }

    _handles_count = _notifiers.count();
}

void register_notifier(Notifier *notifier)
{

    _notifiers.push_back(notifier);

    update_notifier();
}

void unregister_notifier(Notifier *notifier)
{
    _notifiers.remove_all_value(notifier);

    update_notifier();
}

void update_notifier(Handle *handle, PollEvent event)
{
    for (size_t i = 0; i < _notifiers.count(); i++)
    {
        if (_notifiers[i]->handle() == handle &&
            _notifiers[i]->events() & event)
        {
            _notifiers[i]->invoke();
        }
    }
}

/* --- Timers --------------------------------------------------------------- */

static Vector<Timer *> _timers;

void register_timer(Timer *timer)
{
    _timers.push_back(timer);
}

void unregister_timer(Timer *timer)
{
    _timers.remove_value(timer);
}

void update_timers()
{
    TimeStamp current_fire = system_get_ticks();

    auto timers_list_copy = _timers;

    timers_list_copy.foreach ([&](auto timer) {
        if (timer->running() && timer->scheduled() <= current_fire)
        {
            timer->trigger();
            timer->schedule(current_fire + timer->interval());
        }

        return Iteration::CONTINUE;
    });
}

/* --- Invokers ------------------------------------------------------------- */

static Vector<Invoker *> _invoker;

void register_invoker(Invoker *invoker)
{
    _invoker.push_back(invoker);
}

void unregister_invoker(Invoker *invoker)
{
    _invoker.remove_value(invoker);
}

void update_invoker()
{
    _invoker.foreach ([](Invoker *invoker) {
        if (invoker->should_be_invoke_later())
        {
            invoker->invoke();
        }

        return Iteration::CONTINUE;
    });
}

/* --- Loop ----------------------------------------------------------------- */

static bool _is_running = false;
static bool _is_initialize = false;
static int _exit_value = PROCESS_SUCCESS;

static bool _nested_is_running = false;
static int _nested_exit_value = 0;

static Vector<AtExitHook> _atexit_hooks;

void initialize()
{
    assert(!_is_initialize);

    _is_initialize = true;
}

void uninitialize()
{
    assert(_is_initialize);

    for (size_t i = 0; i < _atexit_hooks.count(); i++)
    {
        _atexit_hooks[i]();
    }

    _atexit_hooks.clear();

    _is_initialize = false;
}

static Timeout get_timeout()
{
    Timeout timeout = UINT32_MAX;

    TimeStamp current_tick = system_get_ticks();

    _timers.foreach ([&](auto timer) {
        if (!timer->running() || timer->interval() == 0)
        {
            return Iteration::CONTINUE;
        }

        if (timer->scheduled() < current_tick)
        {
            timeout = 0;
            return Iteration::CONTINUE;
        }

        Timeout remaining = timer->scheduled() - current_tick;

        if (remaining <= timeout)
        {
            timeout = remaining;
        }

        return Iteration::CONTINUE;
    });

    return timeout;
}

void atexit(AtExitHook hook)
{
    _atexit_hooks.push_back(hook);
}

void pump(bool pool)
{
    assert(_is_initialize);

    Timeout timeout = 0;

    if (!pool)
    {
        timeout = get_timeout();
    }

    Handle *selected = nullptr;
    PollEvent selected_events = 0;

    Result result = handle_poll(
        &_handles[0],
        &_events[0],
        _handles_count,
        &selected,
        &selected_events,
        timeout);

    if (result_is_error(result))
    {
        logger_error("Failed to select : %s", result_to_string(result));
        exit(PROCESS_FAILURE);
    }

    update_timers();

    update_notifier(selected, selected_events);

    update_invoker();
}

int run()
{
    assert(_is_initialize);
    assert(!_is_running);

    _is_running = true;

    while (_is_running)
    {
        pump(false);
    }

    uninitialize();

    return _exit_value;
}

void exit(int exit_value)
{
    assert(_is_initialize);

    _is_running = false;
    _exit_value = exit_value;
}

int run_nested()
{
    assert(_is_initialize);
    assert(_is_running);
    assert(!_nested_is_running);

    _nested_is_running = true;

    while (_nested_is_running)
    {
        pump(false);
    }

    return _nested_exit_value;
}

void exit_nested(int exit_value)
{
    assert(_is_initialize);
    assert(_nested_is_running);

    _nested_is_running = false;
    _nested_exit_value = exit_value;
}

} // namespace EventLoop