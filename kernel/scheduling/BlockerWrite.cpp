
#include "kernel/scheduling/Blocker.h"
#include "kernel/tasking/Task.h"

struct BlockerWrite : public Blocker
{
    FsHandle *handle;
};

bool blocker_write_can_unblock(BlockerWrite *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->handle->node) &&
           blocker->handle->node->can_write(blocker->handle);
}

void blocker_write_unblock(BlockerWrite *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->handle->node, task->id);
}

Blocker *blocker_write_create(FsHandle *handle)
{
    assert(handle);
    BlockerWrite *write_blocker = __create(BlockerWrite);

    TASK_BLOCKER(write_blocker)->can_unblock = (BlockerCanUnblockCallback)blocker_write_can_unblock;
    TASK_BLOCKER(write_blocker)->on_unblock = (BlockerUnblockCallback)blocker_write_unblock;

    write_blocker->handle = handle;

    return (Blocker *)write_blocker;
}
