#include "tasking.h"
#include "sheduling/TaskBlockerWrite.h"

bool blocker_write_can_unblock(TaskBlockerWrite *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->node) && fsnode_can_write(blocker->node);
}

void blocker_write_unblock(TaskBlockerWrite *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->node, task->id);
}

TaskBlocker *blocker_write_create(FsNode *node)
{
    TaskBlockerWrite *write_blocker = __create(TaskBlockerWrite);

    write_blocker->blocker = (TaskBlocker){
        (TaskBlockerCanUnblock)blocker_write_can_unblock,
        (TaskBlockerUnblock)blocker_write_unblock,
    };

    write_blocker->node = node;

    return (TaskBlocker *)write_blocker;
}