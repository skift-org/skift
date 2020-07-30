
#include <libsystem/core/CString.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Handle.h"

#define CONNECTION_BUFFER_SIZE 4096

static void fsconnection_accept(FsConnection *connection)
{
    connection->accepted = true;
}

static bool fsconnection_is_accepted(FsConnection *connection)
{
    return connection->accepted;
}

static bool fsconnection_can_read(FsConnection *connection, FsHandle *handle)
{
    if (fshandle_has_flag(handle, OPEN_CLIENT))
        return !ringbuffer_is_empty(connection->data_to_client) || !connection->node.server;
    else
        return !ringbuffer_is_empty(connection->data_to_server) || !connection->node.clients;
}

static Result fsconnection_read(
    FsConnection *connection,
    FsHandle *handle,
    void *buffer,
    size_t size,
    size_t *read)
{
    RingBuffer *data = nullptr;

    if (fshandle_has_flag(handle, OPEN_CLIENT))
    {
        if (!connection->node.server)
        {
            return ERR_STREAM_CLOSED;
        }

        data = connection->data_to_client;
    }
    else
    {
        if (!connection->node.clients)
        {
            return ERR_STREAM_CLOSED;
        }

        data = connection->data_to_server;
    }

    *read = ringbuffer_read(data, (char *)buffer, size);

    return SUCCESS;
}

static Result fsconnection_write(
    FsConnection *connection,
    FsHandle *handle,
    const void *buffer,
    size_t size,
    size_t *written)
{
    RingBuffer *data = nullptr;

    if (fshandle_has_flag(handle, OPEN_CLIENT))
    {
        if (!connection->node.server)
        {
            return ERR_STREAM_CLOSED;
        }

        data = connection->data_to_server;
    }
    else
    {
        if (!connection->node.clients)
        {
            return ERR_STREAM_CLOSED;
        }

        data = connection->data_to_client;
    }

    *written = ringbuffer_write(data, (const char *)buffer, size);

    return SUCCESS;
}

static void fsconnection_destroy(FsConnection *connection)
{
    ringbuffer_destroy(connection->data_to_client);
    ringbuffer_destroy(connection->data_to_server);
}

FsNode *fsconnection_create()
{
    FsConnection *connection = __create(FsConnection);

    fsnode_init(FSNODE(connection), FILE_TYPE_CONNECTION);

    FSNODE(connection)->accept = (FsNodeAcceptCallback)fsconnection_accept;
    FSNODE(connection)->is_accepted = (FsNodeIsAcceptedCallback)fsconnection_is_accepted;
    FSNODE(connection)->can_read = (FsNodeCanReadCallback)fsconnection_can_read;
    FSNODE(connection)->read = (FsNodeReadCallback)fsconnection_read;
    FSNODE(connection)->write = (FsNodeWriteCallback)fsconnection_write;
    FSNODE(connection)->destroy = (FsNodeDestroyCallback)fsconnection_destroy;

    connection->data_to_client = ringbuffer_create(CONNECTION_BUFFER_SIZE);
    connection->data_to_server = ringbuffer_create(CONNECTION_BUFFER_SIZE);

    return FSNODE(connection);
}
