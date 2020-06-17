#include <libsystem/Assert.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Socket.h"

static FsNode *socket_openConnection(FsSocket *socket)
{
    FsNode *connection = fsconnection_create();

    list_pushback(socket->pending, fsnode_ref(connection));

    return connection;
}

static bool socket_FsNodeCanAcceptConnectionCallback(FsSocket *socket)
{
    return list_any(socket->pending);
}

static FsNode *socket_FsNodeAcceptConnectionCallback(FsSocket *socket)
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

static void socket_destroy(FsSocket *socket)
{
    list_destroy_with_callback(socket->pending, (ListDestroyElementCallback)fsnode_deref);
}

FsNode *socket_create(void)
{
    FsSocket *socket = __create(FsSocket);

    fsnode_init(FSNODE(socket), FILE_TYPE_SOCKET);

    FSNODE(socket)->open_connection = (FsNodeOpenConnectionCallback)socket_openConnection;
    FSNODE(socket)->can_accept_connection = (FsNodeCanAcceptConnectionCallback)socket_FsNodeCanAcceptConnectionCallback;
    FSNODE(socket)->accept_connection = (FsNodeAcceptConnectionCallback)socket_FsNodeAcceptConnectionCallback;
    FSNODE(socket)->destroy = (FsNodeDestroyCallback)socket_destroy;

    socket->pending = list_create();

    return FSNODE(socket);
}
