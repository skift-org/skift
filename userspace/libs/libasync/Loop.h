#pragma once

#include <libio/Handle.h>
#include <libutils/Shared.h>
#include <libutils/Vec.h>

namespace Async
{

using AtExitHook = void (*)(void);

struct Notifier;

struct Timer;

struct Invoker;

struct Loop : public Shared<Loop>
{
private:
    bool _is_running = false;
    int _exit_value = PROCESS_SUCCESS;

    bool _nested_is_running = false;
    int _nested_exit_value = 0;

    Vec<HandlePoll> _polls;

    Vec<Notifier *> _notifiers;
    Vec<Timer *> _timers;
    Vec<Invoker *> _invoker;

    void update_polling_list();

    void update_notifier(int id, PollEvent event);

    void update_timers();

    void update_invoker();

    Timeout get_timeout();

public:
    static Ref<Loop> the();

    Loop();

    ~Loop();

    void register_notifier(Notifier *notifier);

    void unregister_notifier(Notifier *notifier);

    void register_timer(Timer *timer);

    void unregister_timer(Timer *timer);

    void register_invoker(Invoker *timer);

    void unregister_invoker(Invoker *timer);

    void atexit(AtExitHook hook);

    void pump(bool pool);

    int run();

    void exit(int exit_value);

    int run_nested();

    void exit_nested(int exit_value);
};

} // namespace Async
