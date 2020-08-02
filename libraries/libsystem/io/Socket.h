#pragma once

#include <abi/Filesystem.h>

struct Connection;

struct Socket;

Socket *socket_open(const char *path, OpenFlag flags);

struct Connection *socket_connect(const char *path);

struct Connection *socket_accept(Socket *socket);

void socket_close(Socket *socket);

void socket_did_connection_open(Socket *socket, struct Connection *connection);

void socket_did_connection_close(Socket *socket, struct Connection *connection);
