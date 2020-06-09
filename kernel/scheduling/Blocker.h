#pragma once

#include <libsystem/Time.h>

#include "kernel/node/Handle.h"

struct Task;
struct Blocker;

typedef bool (*BlockerCanUnblockCallback)(struct Blocker *blocker, struct Task *task);
typedef void (*BlockerUnblockCallback)(struct Blocker *blocker, struct Task *task);
typedef void (*BlockerTimeoutCallback)(struct Blocker *blocker, struct Task *task);

typedef enum
{
    BLOCKER_UNBLOCKED,
    BLOCKER_TIMEOUT,
} BlockerResult;

typedef struct Blocker
{
    BlockerResult result;
    TimeStamp timeout;

    BlockerCanUnblockCallback can_unblock;
    BlockerUnblockCallback on_unblock;
    BlockerTimeoutCallback on_timeout;
} Blocker;

#define TASK_BLOCKER(__subclass) ((Blocker *)(__subclass))

Blocker *blocker_accept_create(FsNode *node);

Blocker *blocker_connect_create(FsNode *connection);

Blocker *blocker_read_create(FsHandle *handle);

Blocker *blocker_select_create(FsHandle **handles, SelectEvent *events, size_t count, FsHandle **selected, SelectEvent *selected_events);

Blocker *blocker_time_create(uint wake_up_tick);

Blocker *blocker_wait_create(struct Task *task, int *exit_value);

Blocker *blocker_write_create(FsHandle *handle);
