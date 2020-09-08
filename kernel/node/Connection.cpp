
#include <libsystem/core/CString.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Handle.h"

#define CONNECTION_BUFFER_SIZE 4096

static void fsconnection_accept(FsConnection *connection)
{
    connection->_accepted = true;
}

static bool fsconnection_is_accepted(FsConnection *connection)
{
    return connection->_accepted;
}

static void fsconnection_destroy(FsConnection *connection)
{
    ringbuffer_destroy(connection->_data_to_client);
    ringbuffer_destroy(connection->_data_to_server);
}

FsConnection::FsConnection() : FsNode(FILE_TYPE_CONNECTION)
{
    accept = (FsNodeAcceptCallback)fsconnection_accept;
    is_accepted = (FsNodeIsAcceptedCallback)fsconnection_is_accepted;
    destroy = (FsNodeDestroyCallback)fsconnection_destroy;

    _data_to_client = ringbuffer_create(CONNECTION_BUFFER_SIZE);
    _data_to_server = ringbuffer_create(CONNECTION_BUFFER_SIZE);
}

bool FsConnection::can_read(FsHandle *handle)
{
    if (handle->has_flag(OPEN_CLIENT))
    {
        return !ringbuffer_is_empty(_data_to_client) || !server;
    }
    else
    {
        return !ringbuffer_is_empty(_data_to_server) || !clients;
    }
}

bool FsConnection::can_write(FsHandle *handle)
{
    if (handle->has_flag(OPEN_CLIENT))
    {
        return !ringbuffer_is_full(_data_to_server) || !server;
    }
    else
    {
        return !ringbuffer_is_full(_data_to_client) || !clients;
    }
}

ResultOr<size_t> FsConnection::read(FsHandle &handle, void *buffer, size_t size)
{
    RingBuffer *data = nullptr;

    if (handle.has_flag(OPEN_CLIENT))
    {
        if (!server)
        {
            return ERR_STREAM_CLOSED;
        }

        data = _data_to_client;
    }
    else
    {
        if (!clients)
        {
            return ERR_STREAM_CLOSED;
        }

        data = _data_to_server;
    }

    return ringbuffer_read(data, (char *)buffer, size);
}

ResultOr<size_t> FsConnection::write(FsHandle &handle, const void *buffer, size_t size)
{
    RingBuffer *data = nullptr;

    if (handle.has_flag(OPEN_CLIENT))
    {
        if (!server)
        {
            return ERR_STREAM_CLOSED;
        }

        data = _data_to_server;
    }
    else
    {
        if (!clients)
        {
            return ERR_STREAM_CLOSED;
        }

        data = _data_to_client;
    }

    return ringbuffer_write(data, (const char *)buffer, size);
}
