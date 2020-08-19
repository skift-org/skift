#include <libsystem/Assert.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Socket.h"

static FsNode *socket_openConnection(FsSocket *socket)
{
    FsNode *connection = fsconnection_create();

    list_pushback(socket->pending, connection->ref());

    return connection;
}

static bool socket_FsNodeCanAcceptConnectionCallback(FsSocket *socket)
{
    return socket->pending->any();
}

static FsNode *socket_FsNodeAcceptConnectionCallback(FsSocket *socket)
{
    assert(socket->pending->any());

    FsNode *connection;
    list_pop(socket->pending, (void **)&connection);

    if (connection->accept)
    {
        connection->accept(connection);
    }

    return connection;
}

static void deref_connection(FsNode *node)
{
    node->deref();
}

static void socket_destroy(FsSocket *socket)
{
    list_destroy_with_callback(socket->pending, (ListDestroyElementCallback)deref_connection);
}

FsNode *socket_create()
{
    FsSocket *socket = __create(FsSocket);

    fsnode_init(socket, FILE_TYPE_SOCKET);

    socket->open_connection = (FsNodeOpenConnectionCallback)socket_openConnection;
    socket->can_accept_connection = (FsNodeCanAcceptConnectionCallback)socket_FsNodeCanAcceptConnectionCallback;
    socket->accept_connection = (FsNodeAcceptConnectionCallback)socket_FsNodeAcceptConnectionCallback;
    socket->destroy = (FsNodeDestroyCallback)socket_destroy;

    socket->pending = list_create();

    return socket;
}
