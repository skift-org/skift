#pragma once

struct task;
struct TaskBlocker;

typedef bool (*TaskBlockerCanUnblock)(struct TaskBlocker *blocker, struct task *task);
typedef void (*TaskBlockerUnblock)(struct TaskBlocker *blocker, struct task *task);

typedef struct TaskBlocker
{
    TaskBlockerCanUnblock can_unblock;
    TaskBlockerUnblock unblock;
} TaskBlocker;