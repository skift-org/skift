
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Terminal.h"

#define TERMINAL_RINGBUFFER_SIZE 1024

static Result terminal_iocall(FsTerminal *terminal, FsHandle *handle, IOCall request, void *args)
{
    __unused(handle);

    IOCallTerminalSizeArgs *size_args = (IOCallTerminalSizeArgs *)args;

    switch (request)
    {
    case IOCALL_TERMINAL_GET_SIZE:
        size_args->width = terminal->width;
        size_args->height = terminal->height;

        return SUCCESS;

    case IOCALL_TERMINAL_SET_SIZE:
        if (size_args->width < 0 || size_args->height < 0)
        {
            return ERR_INVALID_ARGUMENT;
        }

        terminal->width = size_args->width;
        terminal->height = size_args->height;

        return SUCCESS;

    default:
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

static size_t terminal_size(FsTerminal *terminal, FsHandle *handle)
{
    __unused(handle);
    __unused(terminal);

    return TERMINAL_RINGBUFFER_SIZE;
}

static void terminal_destroy(FsTerminal *terminal)
{
    ringbuffer_destroy(terminal->master_to_slave_buffer);
    ringbuffer_destroy(terminal->slave_to_master_buffer);
}

FsTerminal::FsTerminal() : FsNode(FILE_TYPE_TERMINAL)
{
    call = (FsNodeCallCallback)terminal_iocall;
    size = (FsNodeSizeCallback)terminal_size;
    destroy = (FsNodeDestroyCallback)terminal_destroy;

    width = 80;
    width = 25;

    master_to_slave_buffer = ringbuffer_create(TERMINAL_RINGBUFFER_SIZE);
    slave_to_master_buffer = ringbuffer_create(TERMINAL_RINGBUFFER_SIZE);
}

bool FsTerminal::can_read(FsHandle *handle)
{
    __unused(handle);

    if (fshandle_has_flag(handle, OPEN_MASTER))
    {
        return !ringbuffer_is_empty(slave_to_master_buffer) || !writers;
    }
    else
    {
        return !ringbuffer_is_empty(master_to_slave_buffer) || !master;
    }
}

bool FsTerminal::can_write(FsHandle *handle)
{
    __unused(handle);

    if (fshandle_has_flag(handle, OPEN_MASTER))
    {
        return !ringbuffer_is_full(master_to_slave_buffer) || !readers;
    }
    else
    {
        return !ringbuffer_is_full(slave_to_master_buffer) || !master;
    }
}

ResultOr<size_t> FsTerminal::read(FsHandle &handle, void *buffer, size_t size)
{
    __unused(handle);

    size_t read = 0;

    if (fshandle_has_flag(&handle, OPEN_MASTER))
    {
        if (!writers)
        {
            return ERR_STREAM_CLOSED;
        }

        read = ringbuffer_read(slave_to_master_buffer, (char *)buffer, size);
    }
    else
    {
        if (!master)
        {
            return ERR_STREAM_CLOSED;
        }

        read = ringbuffer_read(master_to_slave_buffer, (char *)buffer, size);
    }

    return read;
}

ResultOr<size_t> FsTerminal::write(FsHandle &handle, const void *buffer, size_t size)
{
    __unused(handle);

    size_t written = 0;

    if (fshandle_has_flag(&handle, OPEN_MASTER))
    {
        if (!readers)
        {
            return ERR_STREAM_CLOSED;
        }

        written = ringbuffer_write(master_to_slave_buffer, (const char *)buffer, size);
    }
    else
    {
        if (!master)
        {
            return ERR_STREAM_CLOSED;
        }

        written = ringbuffer_write(slave_to_master_buffer, (const char *)buffer, size);
    }

    return written;
}
