#pragma once

#include <libsystem/runtime.h>

struct Task;
struct TaskBlocker;

typedef bool (*TaskBlockerCanUnblock)(struct TaskBlocker *blocker, struct Task *task);
typedef void (*TaskBlockerUnblock)(struct TaskBlocker *blocker, struct Task *task);

typedef struct TaskBlocker
{
    TaskBlockerCanUnblock can_unblock;
    TaskBlockerUnblock unblock;
} TaskBlocker;