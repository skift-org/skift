#pragma once

#include <libsystem/thread/Lock.h>
#include <libutils/RingBuffer.h>

#include "kernel/node/Node.h"

class FsTerminal : public FsNode
{
private:
public:
    int width;
    int height;

    static constexpr int BUFFER_SIZE = 1024;

    RingBuffer master_to_slave_buffer{BUFFER_SIZE};
    RingBuffer slave_to_master_buffer{BUFFER_SIZE};

    FsTerminal();

    bool can_read(FsHandle *handle);

    bool can_write(FsHandle *handle);

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size);

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size);
};
