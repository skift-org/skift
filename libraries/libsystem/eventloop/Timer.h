#pragma once

#include <libsystem/time.h>

struct Timer;

typedef void (*TimerCallback)(struct Timer *timer);

typedef struct Timer
{
    bool started;

    Timeout elapsed;
    Timeout interval;
    TimerCallback callback;
} Timer;

Timer *timer_create(Timeout interval, TimerCallback callback);

void timer_destroy(Timer *timer);

void timer_start(Timer *timer);

void timer_stop(Timer *timer);
