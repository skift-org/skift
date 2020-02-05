#include "sheduling/TaskBlockerSelect.h"
#include "tasking.h"

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
        SelectEvent event = fshandle_select(blocker->handles[i], blocker->events[i]);

        if (event != 0)
        {
            *blocker->selected = blocker->handles[i];
            *blocker->selected_events = event;

            return;
        }
    }
}

TaskBlocker *blocker_select_create(FsHandle **handles, SelectEvent *events, size_t count, FsHandle **selected, SelectEvent *selected_events)
{
    TaskBlockerSelect *select_blocker = __create(TaskBlockerSelect);

    select_blocker->blocker = (TaskBlocker){
        (TaskBlockerCanUnblock)blocker_select_can_unblock,
        (TaskBlockerUnblock)blocker_select_unblock,
    };

    select_blocker->handles = handles;
    select_blocker->events = events;
    select_blocker->count = count;
    select_blocker->selected = selected;
    select_blocker->selected_events = selected_events;

    return (TaskBlocker *)select_blocker;
}