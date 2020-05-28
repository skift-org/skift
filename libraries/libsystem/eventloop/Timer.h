#pragma once

#include <libsystem/Time.h>

struct Timer;

typedef void (*TimerCallback)(void *target);

typedef struct Timer
{
    void *target;
    TimerCallback callback;

    bool started;
    Timeout interval;
    TimeStamp scheduled;
} Timer;

Timer *timer_create(void *target, Timeout interval, TimerCallback callback);

void timer_destroy(Timer *timer);

void timer_start(Timer *timer);

void timer_stop(Timer *timer);
