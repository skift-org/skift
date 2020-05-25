#pragma once

#include <abi/Handle.h>

struct Socket;

typedef struct Connection
{
    Handle handle;
    struct Socket *socket;
} Connection;

void connection_close(Connection *connection);

size_t connection_send(Connection *connection, const void *buffer, size_t size);

size_t connection_receive(Connection *connection, void *buffer, size_t size);
