/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/sheduling/TaskBlockerAccept.h"
#include "kernel/tasking.h"

bool blocker_accept_can_unblock(TaskBlockerAccept *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->node) && fsnode_can_accept(blocker->node);
}

void blocker_accept_on_unblock(TaskBlockerAccept *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->node, task->id);
}

TaskBlocker *blocker_accept_create(FsNode *node)
{
    TaskBlockerAccept *accept_blocker = __create(TaskBlockerAccept);

    TASK_BLOCKER(accept_blocker)->can_unblock = (TaskBlockerCanUnblockCallback)blocker_accept_can_unblock;
    TASK_BLOCKER(accept_blocker)->on_unblock = (TaskBlockerUnblockCallback)blocker_accept_on_unblock;

    accept_blocker->node = node;

    return (TaskBlocker *)accept_blocker;
}
