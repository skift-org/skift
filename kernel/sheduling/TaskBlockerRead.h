#pragma once

#include "sheduling/TaskBlocker.h"
#include "node/Node.h"

typedef struct
{
    TaskBlocker blocker;
    FsNode *node;
} TaskBlockerRead;

TaskBlocker *blocker_read_create(FsNode *node);