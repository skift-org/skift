#pragma once

#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;
    RingBuffer *buffer;
} FsPipe;

FsNode *pipe_create(void);
