
#include <assert.h>
#include <libsystem/Logger.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Terminal.h"

FsTerminal::FsTerminal() : FsNode(FILE_TYPE_TERMINAL) {}

bool FsTerminal::can_read(FsHandle *handle)
{
    if (handle->has_flag(OPEN_SERVER))
    {
        return !client_to_server_buffer.empty() || !writers();
    }
    else
    {
        return !server_to_client_buffer.empty() || !server();
    }
}

bool FsTerminal::can_write(FsHandle *handle)
{
    if (handle->has_flag(OPEN_SERVER))
    {
        return !server_to_client_buffer.full() || !readers();
    }
    else
    {
        return !client_to_server_buffer.full() || !server();
    }
}

ResultOr<size_t> FsTerminal::read(FsHandle &handle, void *buffer, size_t size)
{
    if (handle.has_flag(OPEN_SERVER))
    {
        if (writers())
        {
            return client_to_server_buffer.read((char *)buffer, size);
        }
        else
        {
            return ERR_STREAM_CLOSED;
        }
    }
    else
    {
        if (server())
        {
            return server_to_client_buffer.read((char *)buffer, size);
        }
        else
        {
            return ERR_STREAM_CLOSED;
        }
    }
}

ResultOr<size_t> FsTerminal::write(FsHandle &handle, const void *buffer, size_t size)
{
    if (handle.has_flag(OPEN_SERVER))
    {
        if (readers())
        {
            return server_to_client_buffer.write((const char *)buffer, size);
        }
        else
        {
            return ERR_STREAM_CLOSED;
        }
    }
    else
    {
        if (server())
        {
            return client_to_server_buffer.write((const char *)buffer, size);
        }
        else
        {
            return ERR_STREAM_CLOSED;
        }
    }
}

Result FsTerminal::call(FsHandle &handle, IOCall request, void *args)
{
    __unused(handle);

    if (!server())
    {
        return ERR_STREAM_CLOSED;
    }

    IOCallTerminalSizeArgs *size_args = (IOCallTerminalSizeArgs *)args;

    switch (request)
    {
    case IOCALL_TERMINAL_GET_SIZE:

        size_args->width = _width;
        size_args->height = _height;

        return SUCCESS;

    case IOCALL_TERMINAL_SET_SIZE:
        if (size_args->width < 0 || size_args->height < 0)
        {
            return ERR_INVALID_ARGUMENT;
        }

        _width = size_args->width;
        _height = size_args->height;

        return SUCCESS;

    default:
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}
