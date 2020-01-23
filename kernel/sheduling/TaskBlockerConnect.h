#pragma once

#include "node/Connection.h"
#include "sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsNode *connection;
} TaskBlockerConnect;

TaskBlocker *blocker_connect_create(FsNode *connection);
