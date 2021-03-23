#pragma once

#include <libutils/RingBuffer.h>

#include "kernel/node/Node.h"

class FsPipe : public FsNode
{
private:
    static constexpr int BUFFER_SIZE = 4096;

    RingBuffer<char> _buffer{BUFFER_SIZE};

public:
    FsPipe();

    bool can_read(FsHandle &handle) override;

    bool can_write(FsHandle &handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size) override;
};
