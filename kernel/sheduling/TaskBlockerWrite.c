/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/sheduling/TaskBlockerWrite.h"
#include "kernel/tasking.h"

bool blocker_write_can_unblock(TaskBlockerWrite *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->handle->node) && fsnode_can_write(blocker->handle->node, blocker->handle);
}

void blocker_write_unblock(TaskBlockerWrite *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->handle->node, task->id);
}

TaskBlocker *blocker_write_create(FsHandle *handle)
{
    TaskBlockerWrite *write_blocker = __create(TaskBlockerWrite);

    TASK_BLOCKER(write_blocker)->can_unblock = (TaskBlockerCanUnblockCallback)blocker_write_can_unblock;
    TASK_BLOCKER(write_blocker)->on_unblock = (TaskBlockerUnblockCallback)blocker_write_unblock;

    write_blocker->handle = handle;

    return (TaskBlocker *)write_blocker;
}