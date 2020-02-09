#pragma once

#include <libsystem/runtime.h>

struct Notifier;

void eventloop_initialize(void);

int eventloop_run(void);

void eventloop_pump(void);

void eventloop_exit(int exit_value);

void eventloop_register_notifier(struct Notifier *notifier);

void eventloop_unregister_notifier(struct Notifier *notifier);
