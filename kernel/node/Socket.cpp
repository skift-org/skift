#include <libsystem/Assert.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Socket.h"

FsSocket::FsSocket() : FsNode(FILE_TYPE_SOCKET)
{
    _pending = list_create();
}

static void deref_connection(FsNode *node)
{
    node->deref();
}

FsSocket::~FsSocket()
{
    list_destroy_with_callback(_pending, (ListDestroyElementCallback)deref_connection);
}

ResultOr<FsNode *> FsSocket::connect()
{
    FsNode *connection = new FsConnection();

    connection->ref();
    list_pushback(_pending, connection);

    return connection;
}

bool FsSocket::can_accept()
{
    return _pending->any();
}

ResultOr<FsNode *> FsSocket::accept()
{
    assert(_pending->any());

    FsNode *connection;
    list_pop(_pending, (void **)&connection);
    connection->accepted();
    return connection;
}
