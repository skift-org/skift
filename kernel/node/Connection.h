#pragma once

#include <libsystem/list.h>

#include "node/Node.h"

typedef struct
{
    FsNode node;

    bool accepted;
    bool is_open;

    List *message_to_server;
    List *message_to_client;
} FsConnection;

FsNode *connection_create(void);