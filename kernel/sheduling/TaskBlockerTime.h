#pragma once

#include "sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    uint wakeup_tick;
} TaskBlockerTime;

TaskBlocker *blocker_time_create(uint wake_up_tick);