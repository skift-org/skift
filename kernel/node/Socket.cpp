#include <libsystem/Assert.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Socket.h"

static FsNode *socket_openConnection(FsSocket *socket)
{
    FsNode *connection = new FsConnection();

    connection->ref();
    list_pushback(socket->pending, connection);

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

FsSocket::FsSocket() : FsNode(FILE_TYPE_SOCKET)
{
    open_connection = (FsNodeOpenConnectionCallback)socket_openConnection;
    can_accept_connection = (FsNodeCanAcceptConnectionCallback)socket_FsNodeCanAcceptConnectionCallback;
    accept_connection = (FsNodeAcceptConnectionCallback)socket_FsNodeAcceptConnectionCallback;

    pending = list_create();
}

FsSocket::~FsSocket()
{
    list_destroy_with_callback(pending, (ListDestroyElementCallback)deref_connection);
}
