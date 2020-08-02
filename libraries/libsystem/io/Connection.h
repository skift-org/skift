#pragma once

#include <abi/Handle.h>

struct Socket;

struct Connection;

Connection *connection_create(struct Socket *socket, Handle handle);

void connection_close(Connection *connection);

size_t connection_send(Connection *connection, const void *buffer, size_t size);

size_t connection_receive(Connection *connection, void *buffer, size_t size);
