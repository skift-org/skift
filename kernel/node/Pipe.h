#pragma once

#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

class FsPipe : public FsNode
{
private:
public:
    RingBuffer *buffer;

    FsPipe();

    bool can_read(FsHandle *handle);

    bool can_write(FsHandle *handle);
};
