/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/sheduling/TaskBlockerRead.h"
#include "kernel/tasking.h"

bool blocker_read_can_unblock(TaskBlockerRead *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->handle->node) && fsnode_can_read(blocker->handle->node, blocker->handle);
}

void blocker_read_unblock(TaskBlockerRead *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->handle->node, task->id);
}

TaskBlocker *blocker_read_create(FsHandle *handle)
{
    TaskBlockerRead *read_blocker = __create(TaskBlockerRead);

    TASK_BLOCKER(read_blocker)->can_unblock = (TaskBlockerCanUnblockCallback)blocker_read_can_unblock;
    TASK_BLOCKER(read_blocker)->on_unblock = (TaskBlockerUnblockCallback)blocker_read_unblock;

    read_blocker->handle = handle;

    return (TaskBlocker *)read_blocker;
}