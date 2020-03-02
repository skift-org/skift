/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/assert.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Socket.h"

FsNode *socket_FsOperationOpenConnection(FsSocket *socket)
{
    FsNode *connection = connection_create();

    list_pushback(socket->pending, fsnode_ref(connection));

    return connection;
}

bool socket_FsOperationCanAcceptConnection(FsSocket *socket)
{
    return list_any(socket->pending);
}

FsNode *socket_FsOperationAcceptConnection(FsSocket *socket)
{
    assert(list_any(socket->pending));

    FsNode *connection;
    list_pop(socket->pending, (void **)&connection);

    if (connection->accept)
    {
        connection->accept(connection);
    }

    return connection;
}

void socket_FsOperationDestroy(FsSocket *socket)
{
    list_destroy_with_callback(socket->pending, (ListDestroyElementCallback)fsnode_deref);
}

FsNode *socket_create(void)
{
    FsSocket *socket = __create(FsSocket);

    fsnode_init(FSNODE(socket), FSNODE_SOCKET);

    FSNODE(socket)->open_connection = (FsOperationOpenConnection)socket_FsOperationOpenConnection;
    FSNODE(socket)->can_accept_connection = (FsOperationCanAcceptConnection)socket_FsOperationCanAcceptConnection;
    FSNODE(socket)->accept_connection = (FsOperationAcceptConnection)socket_FsOperationAcceptConnection;
    FSNODE(socket)->destroy = (FsOperationDestroy)socket_FsOperationDestroy;

    socket->pending = list_create();

    return FSNODE(socket);
}
