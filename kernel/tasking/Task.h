#pragma once

#include <libsystem/lock.h>

#include "node/Handle.h"
#include "platform/PlatformContext.h"
#include "tasking/Stack.h"

#define TASK_MAX_HANDLES 32

typedef struct
{
    int id;
    _Atomic int refcount;

    PlatformContext *context;

    Stack *user_stack;
    Stack *supervisor_stack;

    Lock handles_lock;
    Handle *handles[TASK_MAX_HANDLES];
} Task;

Task *task_create(Task *parent);

void task_destroy(Task *task);

void task_ref(Task *task);

void task_deref(Task *task);
