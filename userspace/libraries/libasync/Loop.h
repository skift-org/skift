#pragma once

#include <libsystem/Common.h>

namespace Async
{

class Notifier;

class Timer;

class Invoker;

namespace Loop
{

/* --- Notifiers ------------------------------------------------------------ */

void register_notifier(Notifier *notifier);

void unregister_notifier(Notifier *notifier);

/* --- Timers --------------------------------------------------------------- */

void register_timer(Timer *timer);

void unregister_timer(Timer *timer);

/* --- Invokers ------------------------------------------------------------- */

void register_invoker(Invoker *timer);

void unregister_invoker(Invoker *timer);

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

} // namespace Loop

} // namespace Async
