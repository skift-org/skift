#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Invoker.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/system/System.h>
#include <libsystem/utils/List.h>
#include <libutils/Assert.h>
#include <libutils/Vector.h>

namespace EventLoop
{

/* --- Notifiers ------------------------------------------------------------ */

static Vector<Notifier *> _notifiers;
static Vector<HandlePoll> _polls;

void update_notifier()
{
    _polls.clear();

    for (Notifier *notifier : _notifiers)
    {
        _polls.push_back({
            notifier->handle()->id(),
            notifier->events(),
            0,
        });
    }
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

void update_notifier(int id, PollEvent event)
{
    for (size_t i = 0; i < _notifiers.count(); i++)
    {
        if (_notifiers[i]->handle()->id() == id &&
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
    Assert::is_false(_is_initialize);

    _is_initialize = true;
}

void uninitialize()
{
    Assert::is_true(_is_initialize);

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
    Assert::is_true(_is_initialize);

    Timeout timeout = 0;

    if (!pool)
    {
        timeout = get_timeout();
    }

    Result result = hj_handle_poll(_polls.raw_storage(), _polls.count(), timeout);

    if (result_is_error(result))
    {
        exit(PROCESS_FAILURE);
    }

    for (const HandlePoll &poll : _polls)
    {
        update_notifier(poll.handle, poll.result);
    }

    update_timers();

    update_invoker();
}

int run()
{
    Assert::is_true(_is_initialize);
    Assert::is_false(_is_running);

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
    Assert::is_true(_is_initialize);

    _is_running = false;
    _exit_value = exit_value;
}

int run_nested()
{
    Assert::is_true(_is_initialize);
    Assert::is_true(_is_running);
    Assert::is_false(_nested_is_running);

    _nested_is_running = true;

    while (_nested_is_running)
    {
        pump(false);
    }

    return _nested_exit_value;
}

void exit_nested(int exit_value)
{
    Assert::is_true(_is_initialize);
    Assert::is_true(_nested_is_running);

    _nested_is_running = false;
    _nested_exit_value = exit_value;
}

} // namespace EventLoop