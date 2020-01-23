#include <libsystem/assert.h>

#include "node/Socket.h"
#include "node/Connection.h"

FsNode *socket_FsOperationOpenConnection(FsSocket *socket)
{
    FsNode *connection = connection_create();

    list_pushback(socket->pending, LIST_KEEP_VALUES);

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
    list_destroy(socket->pending, LIST_KEEP_VALUES);
}

FsNode *socket_create(void)
{
    FsSocket *socket = __create(FsSocket);

    fsnode_init(FSNODE(socket), FSNODE_SOCKET);

    FSNODE(socket)->destroy = (FsOperationDestroy)socket_FsOperationDestroy;

    socket->pending = list_create();

    return FSNODE(socket);
}
