#pragma once

#include <libsystem/Common.h>

class Notifier;

class Timer;

class Invoker;

namespace EventLoop
{

/* --- Notifiers ------------------------------------------------------------ */

void register_notifier(struct Notifier *notifier);

void unregister_notifier(struct Notifier *notifier);

/* --- Timers --------------------------------------------------------------- */

void register_timer(struct Timer *timer);

void unregister_timer(struct Timer *timer);

/* --- Invokers ------------------------------------------------------------- */

void register_invoker(struct Invoker *timer);

void unregister_invoker(struct Invoker *timer);

/* --- Loop ----------------------------------------------------------------- */

using AtExitHook = void (*)(void);

void initialize();

void uninitialize();

void atexit(AtExitHook hook);

void pump(bool pool);

int run();

void exit(int exit_value);

int run_nested();

void exit_nested(int exit_value);

} // namespace EventLoop
