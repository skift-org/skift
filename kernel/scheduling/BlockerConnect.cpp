
#include "kernel/scheduling/Blocker.h"
#include "kernel/tasking/Task.h"

struct BlockerConnect
{
    Blocker blocker;
    FsNode *connection;
};

static bool blocker_connect_can_unblock(BlockerConnect *blocker, Task *task)
{
    __unused(task);

    return fsnode_is_accepted(blocker->connection);
}

static void blocker_connect_unblock(BlockerConnect *blocker, Task *task)
{
    __unused(blocker);
    __unused(task);
}

Blocker *blocker_connect_create(FsNode *connection)
{
    BlockerConnect *connect_blocker = __create(BlockerConnect);

    TASK_BLOCKER(connect_blocker)->can_unblock = (BlockerCanUnblockCallback)blocker_connect_can_unblock;
    TASK_BLOCKER(connect_blocker)->on_unblock = (BlockerUnblockCallback)blocker_connect_unblock;

    connect_blocker->connection = connection;

    return (Blocker *)connect_blocker;
}
