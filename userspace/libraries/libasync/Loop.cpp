#include <libasync/Invoker.h>
#include <libasync/Loop.h>
#include <libasync/Notifier.h>
#include <libasync/Timer.h>
#include <libsystem/system/System.h>
#include <libutils/Assert.h>
#include <libutils/Vector.h>

namespace Async
{

/* --- Notifiers ------------------------------------------------------------ */

static RefPtr<Loop> _instance = nullptr;

RefPtr<Loop> Loop::the()
{
    if (_instance == nullptr)
    {
        _instance = make<Loop>();
    }

    return _instance;
}

void Loop::update_notifier(int id, PollEvent event)
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

void Loop::update_polling_list()
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

void Loop::register_notifier(Notifier *notifier)
{

    _notifiers.push_back(notifier);

    update_polling_list();
}

void Loop::unregister_notifier(Notifier *notifier)
{
    _notifiers.remove_all_value(notifier);

    update_polling_list();
}

/* --- Timers --------------------------------------------------------------- */

void Loop::register_timer(Timer *timer)
{
    _timers.push_back(timer);
}

void Loop::unregister_timer(Timer *timer)
{
    _timers.remove_value(timer);
}

void Loop::update_timers()
{
    TimeStamp current_fire = system_get_ticks();

    auto timers_list_copy = _timers;

    timers_list_copy.foreach([&](auto timer) {
        if (timer->running() && timer->scheduled() <= current_fire)
        {
            timer->trigger();
            timer->schedule(current_fire + timer->interval());
        }

        return Iteration::CONTINUE;
    });
}

/* --- Invokers ------------------------------------------------------------- */

void Loop::register_invoker(Invoker *invoker)
{
    _invoker.push_back(invoker);
}

void Loop::unregister_invoker(Invoker *invoker)
{
    _invoker.remove_value(invoker);
}

void Loop::update_invoker()
{
    _invoker.foreach([](Invoker *invoker) {
        if (invoker->should_be_invoke_later())
        {
            invoker->invoke();
        }

        return Iteration::CONTINUE;
    });
}

/* --- Loop ----------------------------------------------------------------- */

static Vector<AtExitHook> _atexit_hooks;

Loop::Loop()
{
}

Loop::~Loop()
{
    for (size_t i = 0; i < _atexit_hooks.count(); i++)
    {
        _atexit_hooks[i]();
    }
}

Timeout Loop::get_timeout()
{
    Timeout timeout = UINT32_MAX;

    TimeStamp current_tick = system_get_ticks();

    _timers.foreach([&](auto timer) {
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

void Loop::atexit(AtExitHook hook)
{
    _atexit_hooks.push_back(hook);
}

void Loop::pump(bool pool)
{

    Timeout timeout = 0;

    if (!pool)
    {
        timeout = get_timeout();
    }

    HjResult result = hj_handle_poll(_polls.raw_storage(), _polls.count(), timeout);

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

int Loop::run()
{
    Assert::falsity(_is_running);

    _is_running = true;

    while (_is_running)
    {
        pump(false);
    }

    return _exit_value;
}

void Loop::exit(int exit_value)
{

    _is_running = false;
    _exit_value = exit_value;
}

int Loop::run_nested()
{
    Assert::truth(_is_running);
    Assert::falsity(_nested_is_running);

    _nested_is_running = true;

    while (_nested_is_running)
    {
        pump(false);
    }

    return _nested_exit_value;
}

void Loop::exit_nested(int exit_value)
{
    Assert::truth(_nested_is_running);

    _nested_is_running = false;
    _nested_exit_value = exit_value;
}

} // namespace Async
