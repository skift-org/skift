#pragma once

#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

class FsConnection : public FsNode
{
private:
public:
    bool accepted;

    RingBuffer *data_to_server;
    RingBuffer *data_to_client;

    FsConnection();

    bool can_read(FsHandle *handle);

    bool can_write(FsHandle *handle);

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size);

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size);
};
