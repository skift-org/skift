#pragma once

#include <libutils/RingBuffer.h>

#include "system/node/Node.h"

struct FsTerminal : public FsNode
{
private:
    static constexpr int BUFFER_SIZE = 1024;

    int _width = 80;
    int _height = 25;

public:
    RingBuffer<char> server_to_client_buffer{BUFFER_SIZE};
    RingBuffer<char> client_to_server_buffer{BUFFER_SIZE};

    FsTerminal();

    bool can_read(FsHandle &handle) override;

    bool can_write(FsHandle &handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size) override;

    HjResult call(FsHandle &handle, IOCall request, void *args) override;
};
