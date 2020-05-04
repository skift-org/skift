#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/time.h>

struct Timer;

typedef void (*TimerCallback)(void *target);

typedef struct Timer
{
    void *target;
    TimerCallback callback;

    bool started;
    Timeout interval;
    Timeout elapsed;
} Timer;

Timer *timer_create(void *target, Timeout interval, TimerCallback callback);

void timer_destroy(Timer *timer);

void timer_start(Timer *timer);

void timer_stop(Timer *timer);
