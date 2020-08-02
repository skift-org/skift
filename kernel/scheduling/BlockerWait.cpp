#include "kernel/scheduling/Blocker.h"
#include "kernel/tasking/Task.h"

struct BlockerWait : public Blocker
{
    Task *task;
    int *exit_value;
};

static bool blocker_wait_can_unblock(BlockerWait *blocker, Task *task)
{
    __unused(task);
    return blocker->task->state == TASK_STATE_CANCELED;
}

static void blocker_wait_unblock(BlockerWait *blocker, Task *task)
{
    __unused(task);

    *blocker->exit_value = blocker->task->exit_value;
}

Blocker *blocker_wait_create(Task *task, int *exit_value)
{
    BlockerWait *wait_blocker = __create(BlockerWait);

    TASK_BLOCKER(wait_blocker)->can_unblock = (BlockerCanUnblockCallback)blocker_wait_can_unblock;
    TASK_BLOCKER(wait_blocker)->on_unblock = (BlockerUnblockCallback)blocker_wait_unblock;

    wait_blocker->task = task;
    wait_blocker->exit_value = exit_value;

    return (Blocker *)wait_blocker;
}
