#pragma once

#include <libsystem/Common.h>

class Notifier;

class Timer;

class Invoker;

void eventloop_initialize();

int eventloop_run();

int eventloop_run_nested();

void eventloop_pump(bool pool);

void eventloop_exit(int exit_value);

void eventloop_exit_nested(int exit_value);

void eventloop_register_notifier(struct Notifier *notifier);

void eventloop_unregister_notifier(struct Notifier *notifier);

void eventloop_register_timer(struct Timer *timer);

void eventloop_unregister_timer(struct Timer *timer);

void eventloop_register_invoker(struct Invoker *timer);

void eventloop_unregister_invoker(struct Invoker *timer);

void eventloop_update_timers();
