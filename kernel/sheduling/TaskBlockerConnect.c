/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/sheduling/TaskBlockerConnect.h"
#include "kernel/tasking.h"

bool blocker_connect_can_unblock(TaskBlockerConnect *blocker, Task *task)
{
    __unused(task);

    return fsnode_is_accepted(blocker->connection);
}

void blocker_connect_unblock(TaskBlockerConnect *blocker, Task *task)
{
    __unused(blocker);
    __unused(task);
}

TaskBlocker *blocker_connect_create(FsNode *connection)
{
    TaskBlockerConnect *connect_blocker = __create(TaskBlockerConnect);

    TASK_BLOCKER(connect_blocker)->can_unblock = (TaskBlockerCanUnblockCallback)blocker_connect_can_unblock;
    TASK_BLOCKER(connect_blocker)->on_unblock = (TaskBlockerUnblockCallback)blocker_connect_unblock;

    connect_blocker->connection = connection;

    return (TaskBlocker *)connect_blocker;
}
