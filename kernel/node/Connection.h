#pragma once

#include <libsystem/utils/RingBuffer.h>

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;

    bool accepted;
    bool is_open;

    RingBuffer *data_to_server;
    RingBuffer *data_to_client;
} FsConnection;

FsNode *fsconnection_create(void);
