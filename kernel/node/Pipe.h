#pragma once

#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

struct FsPipe : public FsNode
{
    RingBuffer *buffer;

    FsPipe();
};
