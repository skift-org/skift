#include "tasking.h"
#include "sheduling/TaskBlockerRead.h"

bool blocker_read_can_unblock(TaskBlockerRead *blocker, task_t *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->node) && fsnode_can_read(blocker->node);
}

void blocker_read_unblock(TaskBlockerRead *blocker, task_t *task)
{
    fsnode_acquire_lock(blocker->node, task->id);
}

TaskBlocker *blocker_read_create(FsNode *node)
{
    TaskBlockerRead *read_blocker = __create(TaskBlockerRead);

    read_blocker->blocker = (TaskBlocker){
        (TaskBlockerCanUnblock)blocker_read_can_unblock,
        (TaskBlockerUnblock)blocker_read_unblock,
    };

    read_blocker->node = node;

    return (TaskBlocker *)read_blocker;
}