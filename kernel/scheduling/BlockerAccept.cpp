
#include "kernel/scheduling/Blocker.h"
#include "kernel/tasking/Task.h"

typedef struct
{
    Blocker blocker;
    FsNode *node;
} BlockerAccept;

static bool blocker_accept_can_unblock(BlockerAccept *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->node) && fsnode_can_accept(blocker->node);
}

static void blocker_accept_on_unblock(BlockerAccept *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->node, task->id);
}

Blocker *blocker_accept_create(FsNode *node)
{
    BlockerAccept *accept_blocker = __create(BlockerAccept);

    TASK_BLOCKER(accept_blocker)->can_unblock = (BlockerCanUnblockCallback)blocker_accept_can_unblock;
    TASK_BLOCKER(accept_blocker)->on_unblock = (BlockerUnblockCallback)blocker_accept_on_unblock;

    accept_blocker->node = node;

    return (Blocker *)accept_blocker;
}
