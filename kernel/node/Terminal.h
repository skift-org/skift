#pragma once

#include <libsystem/thread/Lock.h>
#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

struct FsTerminalEndpoint;

typedef struct
{
    FsNode node;

    int width;
    int height;

    RingBuffer *master_to_slave_buffer;
    RingBuffer *slave_to_master_buffer;
} FsTerminal;

FsNode *terminal_create();
