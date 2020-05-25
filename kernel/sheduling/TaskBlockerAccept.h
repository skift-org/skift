#pragma once

#include "kernel/node/Node.h"
#include "kernel/sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsNode *node;
} TaskBlockerAccept;

TaskBlocker *blocker_accept_create(FsNode *node);
