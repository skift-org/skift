
#include "kernel/sheduling/Blocker.h"
#include "kernel/tasking.h"

typedef struct
{
    Blocker blocker;
    FsHandle *handle;
} BlockerWrite;

bool blocker_write_can_unblock(BlockerWrite *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->handle->node) && fsnode_can_write(blocker->handle->node, blocker->handle);
}

void blocker_write_unblock(BlockerWrite *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->handle->node, task->id);
}

Blocker *blocker_write_create(FsHandle *handle)
{
    BlockerWrite *write_blocker = __create(BlockerWrite);

    TASK_BLOCKER(write_blocker)->can_unblock = (BlockerCanUnblockCallback)blocker_write_can_unblock;
    TASK_BLOCKER(write_blocker)->on_unblock = (BlockerUnblockCallback)blocker_write_unblock;

    write_blocker->handle = handle;

    return (Blocker *)write_blocker;
}
