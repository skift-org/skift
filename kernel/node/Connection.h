#pragma once

#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

struct FsConnection : public FsNode
{
    bool accepted;
    bool is_open;

    RingBuffer *data_to_server;
    RingBuffer *data_to_client;
};

FsNode *fsconnection_create();
