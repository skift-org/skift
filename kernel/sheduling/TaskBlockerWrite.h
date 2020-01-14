#pragma once

#include "sheduling/TaskBlocker.h"
#include "node/Node.h"

typedef struct
{
    TaskBlocker blocker;
    FsNode *node;
} TaskBlockerWrite;

TaskBlocker *blocker_write_create(FsNode *node);
