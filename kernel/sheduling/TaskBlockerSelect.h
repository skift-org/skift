#pragma once

#pragma once

#include "node/Handle.h"
#include "sheduling/TaskBlocker.h"

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
