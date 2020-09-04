
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
        if (!connection->server)
        {
            return ERR_STREAM_CLOSED;
        }

        data = connection->data_to_client;
    }
    else
    {
        if (!connection->clients)
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
        if (!connection->server)
        {
            return ERR_STREAM_CLOSED;
        }

        data = connection->data_to_server;
    }
    else
    {
        if (!connection->clients)
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

FsConnection::FsConnection() : FsNode(FILE_TYPE_CONNECTION)
{
    accept = (FsNodeAcceptCallback)fsconnection_accept;
    is_accepted = (FsNodeIsAcceptedCallback)fsconnection_is_accepted;
    read = (FsNodeReadCallback)fsconnection_read;
    write = (FsNodeWriteCallback)fsconnection_write;
    destroy = (FsNodeDestroyCallback)fsconnection_destroy;

    data_to_client = ringbuffer_create(CONNECTION_BUFFER_SIZE);
    data_to_server = ringbuffer_create(CONNECTION_BUFFER_SIZE);
}

bool FsConnection::can_read(FsHandle *handle)
{
    if (fshandle_has_flag(handle, OPEN_CLIENT))
    {
        return !ringbuffer_is_empty(data_to_client) || !server;
    }
    else
    {
        return !ringbuffer_is_empty(data_to_server) || !clients;
    }
}
