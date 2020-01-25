#pragma once

#include "abi/Handle.h"
#include "abi/Message.h"

struct Socket;

typedef struct Connection
{
    Handle handle;
    struct Socket *socket;
} Connection;

void connection_close(Connection *connection);

void connection_send(Connection *connection, Message *message);

void connection_receive(Connection *connection, Message *message);

void connection_payload(Connection *connection, Message *message);

void connection_discard(Connection *connection);
