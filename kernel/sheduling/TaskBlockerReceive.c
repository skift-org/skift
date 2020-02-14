/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/sheduling/TaskBlockerReceive.h"
#include "kernel/tasking.h"

bool blocker_receive_can_unblock(TaskBlockerReceive *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->handle->node) && fsnode_can_receive(blocker->handle->node, blocker->handle);
}

void blocker_receive_unblock(TaskBlockerReceive *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->handle->node, task->id);
}

TaskBlocker *blocker_receive_create(FsHandle *handle)
{
    TaskBlockerReceive *receive_blocker = __create(TaskBlockerReceive);

    TASK_BLOCKER(receive_blocker)->can_unblock = (TaskBlockerCanUnblockCallback)blocker_receive_can_unblock;
    TASK_BLOCKER(receive_blocker)->on_unblock = (TaskBlockerUnblockCallback)blocker_receive_unblock;

    receive_blocker->handle = handle;

    return (TaskBlocker *)receive_blocker;
}
