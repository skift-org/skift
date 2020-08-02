#include "kernel/scheduling/Blocker.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Task.h"

struct BlockerTime : public Blocker
{
    uint wakeup_tick;
};

bool blocker_time_can_unblock(BlockerTime *blocker, Task *task)
{
    __unused(task);

    return system_get_tick() >= blocker->wakeup_tick;
}

void blocker_time_unblock(BlockerTime *blocker, Task *task)
{
    __unused(blocker);
    __unused(task);
}

Blocker *blocker_time_create(uint wakeup_tick)
{
    BlockerTime *time_blocker = __create(BlockerTime);

    TASK_BLOCKER(time_blocker)->can_unblock = (BlockerCanUnblockCallback)blocker_time_can_unblock;
    TASK_BLOCKER(time_blocker)->on_unblock = (BlockerUnblockCallback)blocker_time_unblock;

    time_blocker->wakeup_tick = wakeup_tick;

    return (Blocker *)time_blocker;
}
