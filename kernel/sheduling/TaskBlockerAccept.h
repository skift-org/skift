#pragma once

#include "node/Node.h"
#include "sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsNode *node;
} TaskBlockerAccept;

TaskBlocker *blocker_accept_create(FsNode *node);
