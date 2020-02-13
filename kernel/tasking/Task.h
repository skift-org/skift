#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/lock.h>

#include "kernel/node/Handle.h"
#include "kernel/tasking/Stack.h"
#include "platform/PlatformContext.h"

#define TASK_MAX_HANDLES 32

typedef struct
{
    int id;
    _Atomic int refcount;

    PlatformContext *context;

    Stack *user_stack;
    Stack *supervisor_stack;

    Lock handles_lock;
    FsHandle *handles[TASK_MAX_HANDLES];
} Task;

Task *task_create(Task *parent);

void task_destroy(Task *task);

void task_ref(Task *task);

void task_deref(Task *task);
