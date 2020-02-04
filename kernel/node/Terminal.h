#pragma once

#include "node/Node.h"

#include <libsystem/RingBuffer.h>
#include <libsystem/lock.h>

struct FsTerminalEndpoint;

typedef struct
{
    FsNode node;

    int width;
    int height;

    RingBuffer *master_to_slave_buffer;
    RingBuffer *slave_to_master_buffer;
} FsTerminal;

FsNode *terminal_create(void);