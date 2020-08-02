
#include "kernel/scheduling/Blocker.h"
#include "kernel/tasking/Task.h"

struct BlockerSelect : public Blocker
{
    FsHandle **handles;
    SelectEvent *events;
    size_t count;

    FsHandle **selected;
    SelectEvent *selected_events;
};

static bool blocker_select_can_unblock(BlockerSelect *blocker, Task *task)
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

static void blocker_select_unblock(BlockerSelect *blocker, Task *task)
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

Blocker *blocker_select_create(FsHandle **handles, SelectEvent *events, size_t count, FsHandle **selected, SelectEvent *selected_events)
{
    BlockerSelect *select_blocker = __create(BlockerSelect);

    TASK_BLOCKER(select_blocker)->can_unblock = (BlockerCanUnblockCallback)blocker_select_can_unblock;
    TASK_BLOCKER(select_blocker)->on_unblock = (BlockerUnblockCallback)blocker_select_unblock;

    select_blocker->handles = handles;
    select_blocker->events = events;
    select_blocker->count = count;
    select_blocker->selected = selected;
    select_blocker->selected_events = selected_events;

    return (Blocker *)select_blocker;
}
