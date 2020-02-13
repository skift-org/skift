#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/node/Handle.h"
#include "kernel/sheduling/TaskBlocker.h"

typedef struct
{
    TaskBlocker blocker;
    FsHandle *handle;
} TaskBlockerReceive;

TaskBlocker *blocker_receive_create(FsHandle *handle);
