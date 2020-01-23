#pragma once

#include "node/Handle.h"
#include "sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsHandle *handle;
} TaskBlockerReceive;

TaskBlocker *blocker_receive_create(FsHandle *handle);
