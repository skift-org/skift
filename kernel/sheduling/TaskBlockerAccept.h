#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/node/Node.h"
#include "kernel/sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsNode *node;
} TaskBlockerAccept;

TaskBlocker *blocker_accept_create(FsNode *node);
