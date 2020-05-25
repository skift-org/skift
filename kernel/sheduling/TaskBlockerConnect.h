#pragma once

#include "kernel/node/Connection.h"
#include "kernel/sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsNode *connection;
} TaskBlockerConnect;

TaskBlocker *blocker_connect_create(FsNode *connection);
