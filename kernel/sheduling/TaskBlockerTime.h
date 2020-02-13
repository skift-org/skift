#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    uint wakeup_tick;
} TaskBlockerTime;

TaskBlocker *blocker_time_create(uint wake_up_tick);