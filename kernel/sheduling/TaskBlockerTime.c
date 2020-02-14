/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/sheduling/TaskBlockerTime.h"
#include "kernel/tasking.h"

bool blocker_time_can_unblock(TaskBlockerTime *blocker, Task *task)
{
    __unused(task);

    return sheduler_get_ticks() >= blocker->wakeup_tick;
}

void blocker_time_unblock(TaskBlockerTime *blocker, Task *task)
{
    __unused(blocker);
    __unused(task);
}

TaskBlocker *blocker_time_create(uint wakeup_tick)
{
    TaskBlockerTime *time_blocker = __create(TaskBlockerTime);

    TASK_BLOCKER(time_blocker)->can_unblock = (TaskBlockerCanUnblockCallback)blocker_time_can_unblock;
    TASK_BLOCKER(time_blocker)->on_unblock = (TaskBlockerUnblockCallback)blocker_time_unblock;

    time_blocker->wakeup_tick = wakeup_tick;

    return (TaskBlocker *)time_blocker;
}