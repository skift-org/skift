/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/sheduling/TaskBlockerSelect.h"
#include "kernel/tasking.h"

bool blocker_select_can_unblock(TaskBlockerSelect *blocker, Task *task)
{
    __unused(task);

    for (size_t i = 0; i < blocker->count; i++)
    {
        if (fshandle_select(blocker->handles[i], blocker->events[i]) != 0)
        {
            return true;
        }
    }

    return false;
}

void blocker_select_unblock(TaskBlockerSelect *blocker, Task *task)
{
    __unused(blocker);
    __unused(task);

    for (size_t i = 0; i < blocker->count; i++)
    {
        SelectEvent events = fshandle_select(blocker->handles[i], blocker->events[i]);

        if (events != 0)
        {
            *blocker->selected = blocker->handles[i];
            *blocker->selected_events = events;

            return;
        }
    }
}

TaskBlocker *blocker_select_create(FsHandle **handles, SelectEvent *events, size_t count, FsHandle **selected, SelectEvent *selected_events)
{
    TaskBlockerSelect *select_blocker = __create(TaskBlockerSelect);

    TASK_BLOCKER(select_blocker)->can_unblock = (TaskBlockerCanUnblockCallback)blocker_select_can_unblock;
    TASK_BLOCKER(select_blocker)->on_unblock = (TaskBlockerUnblockCallback)blocker_select_unblock;

    select_blocker->handles = handles;
    select_blocker->events = events;
    select_blocker->count = count;
    select_blocker->selected = selected;
    select_blocker->selected_events = selected_events;

    return (TaskBlocker *)select_blocker;
}