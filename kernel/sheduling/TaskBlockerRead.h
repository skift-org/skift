#pragma once

#include "kernel/node/Handle.h"
#include "kernel/sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsHandle *handle;
} TaskBlockerRead;

TaskBlocker *blocker_read_create(FsHandle *handle);
