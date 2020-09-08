#pragma once

#include <libutils/RingBuffer.h>

#include "kernel/node/Node.h"

class FsConnection : public FsNode
{
private:
public:
    static constexpr int BUFFER_SIZE = 4096;

    bool _accepted = false;

    RingBuffer _data_to_server{BUFFER_SIZE};

    RingBuffer _data_to_client{BUFFER_SIZE};

    FsConnection();

    bool can_read(FsHandle *handle);

    bool can_write(FsHandle *handle);

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size);

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size);
};
