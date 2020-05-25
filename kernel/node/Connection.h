#pragma once

#include <libsystem/RingBuffer.h>

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;

    bool accepted;
    bool is_open;

    RingBuffer *data_to_server;
    RingBuffer *data_to_client;
} FsConnection;

FsNode *connection_create(void);
