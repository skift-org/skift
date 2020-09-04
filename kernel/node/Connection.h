#pragma once

#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

class FsConnection : public FsNode
{
private:
public:
    bool accepted;
    bool is_open;

    RingBuffer *data_to_server;
    RingBuffer *data_to_client;

    FsConnection();

    bool can_read(FsHandle *handle);
};
