#pragma once

#include "node/Handle.h"
#include "sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsHandle *handle;
} TaskBlockerWrite;

TaskBlocker *blocker_write_create(FsHandle *handle);
