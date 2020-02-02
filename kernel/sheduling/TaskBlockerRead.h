#pragma once

#include "node/Handle.h"
#include "sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsHandle *handle;
} TaskBlockerRead;

TaskBlocker *blocker_read_create(FsHandle *handle);