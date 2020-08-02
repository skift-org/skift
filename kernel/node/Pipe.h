#pragma once

#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

struct FsPipe
{
    FsNode node;
    RingBuffer *buffer;
} ;

FsNode *fspipe_create();
