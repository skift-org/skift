#pragma once

#include "kernel/node/Handle.h"
#include "kernel/sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsHandle *handle;
} TaskBlockerWrite;

TaskBlocker *blocker_write_create(FsHandle *handle);
