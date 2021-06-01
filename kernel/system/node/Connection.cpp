
#include <string.h>

#include "system/node/Connection.h"
#include "system/node/Handle.h"

#define CONNECTION_BUFFER_SIZE 4096

FsConnection::FsConnection() : FsNode(HJ_FILE_TYPE_CONNECTION) {}

void FsConnection::accepted()
{
    _accepted = true;
}

bool FsConnection::is_accepted()
{
    return _accepted;
}

bool FsConnection::can_read(FsHandle &handle)
{
    if (handle.has_flag(HJ_OPEN_CLIENT))
    {
        return !_data_to_client.empty() || !server();
    }
    else
    {
        return !_data_to_server.empty() || !clients();
    }
}

bool FsConnection::can_write(FsHandle &handle)
{
    if (handle.has_flag(HJ_OPEN_CLIENT))
    {
        return !_data_to_server.full() || !server();
    }
    else
    {
        return !_data_to_client.full() || !clients();
    }
}

ResultOr<size_t> FsConnection::read(FsHandle &handle, void *buffer, size_t size)
{
    if (handle.has_flag(HJ_OPEN_CLIENT))
    {
        if (server())
        {
            return _data_to_client.read((char *)buffer, size);
        }
        else
        {
            return ERR_STREAM_CLOSED;
        }
    }
    else
    {
        if (clients())
        {
            return _data_to_server.read((char *)buffer, size);
        }
        else
        {
            return ERR_STREAM_CLOSED;
        }
    }
}

ResultOr<size_t> FsConnection::write(FsHandle &handle, const void *buffer, size_t size)
{
    if (handle.has_flag(HJ_OPEN_CLIENT))
    {
        if (server())
        {
            return _data_to_server.write((const char *)buffer, size);
        }
        else
        {
            return ERR_STREAM_CLOSED;
        }
    }
    else
    {
        if (clients())
        {
            return _data_to_client.write((const char *)buffer, size);
        }
        else
        {
            return ERR_STREAM_CLOSED;
        }
    }
}
