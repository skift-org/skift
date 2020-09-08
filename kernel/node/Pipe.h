#pragma once

#include <libutils/RingBuffer.h>

#include "kernel/node/Node.h"

class FsPipe : public FsNode
{
private:
public:
    static constexpr int BUFFER_SIZE = 4096;

    RingBuffer _buffer{BUFFER_SIZE};

    FsPipe();

    bool can_read(FsHandle *handle);

    bool can_write(FsHandle *handle);

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size);

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size);
};
