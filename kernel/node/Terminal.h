#pragma once

#include <libsystem/thread/Lock.h>
#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

struct FsTerminalEndpoint;

struct FsTerminal : public FsNode
{
    int width;
    int height;

    RingBuffer *master_to_slave_buffer;
    RingBuffer *slave_to_master_buffer;
};

FsNode *terminal_create();
