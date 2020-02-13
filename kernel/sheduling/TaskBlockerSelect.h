#pragma once

#pragma once

#include "kernel/node/Handle.h"
#include "kernel/sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;

    FsHandle **handles;
    SelectEvent *events;
    size_t count;

    FsHandle **selected;
    SelectEvent *selected_events;
} TaskBlockerSelect;

TaskBlocker *blocker_select_create(FsHandle **handles, SelectEvent *events, size_t count, FsHandle **selected, SelectEvent *selected_events);
