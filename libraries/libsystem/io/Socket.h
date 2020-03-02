#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Filesystem.h>
#include <abi/Handle.h>

#include <libsystem/utils/List.h>

struct Connection;

typedef struct Socket
{
    Handle handle;
    List *connections;
} Socket;

Socket *socket_open(const char *path, OpenFlag flags);

struct Connection *socket_connect(const char *path);

struct Connection *socket_accept(Socket *socket);

void socket_close(Socket *socket);
