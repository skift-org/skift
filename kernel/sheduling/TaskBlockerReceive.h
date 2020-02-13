#pragma once

#include "kernel/node/Handle.h"
#include "kernel/sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsHandle *handle;
} TaskBlockerReceive;

TaskBlocker *blocker_receive_create(FsHandle *handle);
