
#include "kernel/scheduling/Blocker.h"
#include "kernel/tasking/Task.h"

struct BlockerRead : public Blocker
{
    FsHandle *handle;
};

static bool blocker_read_can_unblock(BlockerRead *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->handle->node) && blocker->handle->node->can_read(blocker->handle);
}

static void blocker_read_unblock(BlockerRead *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->handle->node, task->id);
}

Blocker *blocker_read_create(FsHandle *handle)
{
    BlockerRead *read_blocker = __create(BlockerRead);

    TASK_BLOCKER(read_blocker)->can_unblock = (BlockerCanUnblockCallback)blocker_read_can_unblock;
    TASK_BLOCKER(read_blocker)->on_unblock = (BlockerUnblockCallback)blocker_read_unblock;

    read_blocker->handle = handle;

    return (Blocker *)read_blocker;
}
