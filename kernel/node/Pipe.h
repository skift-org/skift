#pragma once

#include <libsystem/RingBuffer.h>

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;
    RingBuffer *buffer;
} FsPipe;

FsNode *pipe_create(void);
