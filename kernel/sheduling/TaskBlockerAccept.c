#include "sheduling/TaskBlockerAccept.h"
#include "tasking.h"

bool blocker_accept_can_unblock(TaskBlockerAccept *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->node) && fsnode_can_accept(blocker->node);
}

void blocker_accept_unblock(TaskBlockerAccept *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->node, task->id);
}

TaskBlocker *blocker_accept_create(FsNode *node)
{
    TaskBlockerAccept *accept_blocker = __create(TaskBlockerAccept);

    accept_blocker->blocker = (TaskBlocker){
        (TaskBlockerCanUnblock)blocker_accept_can_unblock,
        (TaskBlockerUnblock)blocker_accept_unblock,
    };

    accept_blocker->node = node;

    return (TaskBlocker *)accept_blocker;
}
