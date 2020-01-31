#include "sheduling/TaskBlockerSelect.h"
#include "tasking.h"

bool blocker_select_can_unblock(TaskBlockerSelect *blocker, Task *task)
{
    __unused(task);

    for (size_t i = 0; i < blocker->count; i++)
    {
        if (fshandle_can_select(blocker->handles[i], blocker->events[i]))
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
        if (fshandle_can_select(blocker->handles[i], blocker->events[i]))
        {
            if (fshandle_can_select(blocker->handles[i], blocker->events[i]))
            {
                *blocker->selected = blocker->handles[i];

                return;
            }
        }
    }
}

TaskBlocker *blocker_select_create(FsHandle **handles, SelectEvent *events, size_t count, FsHandle **selected)
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

    return (TaskBlocker *)select_blocker;
}