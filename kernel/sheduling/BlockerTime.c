
#include "kernel/sheduling/Blocker.h"
#include "kernel/tasking.h"

typedef struct
{
    Blocker blocker;
    uint wakeup_tick;
} BlockerTime;

bool blocker_time_can_unblock(BlockerTime *blocker, Task *task)
{
    __unused(task);

    return scheduler_get_ticks() >= blocker->wakeup_tick;
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
