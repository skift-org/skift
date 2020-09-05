
#include <libsystem/Result.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Pipe.h"

#define PIPE_BUFFER_SIZE 4096

static size_t pipe_size(FsPipe *node, FsHandle *handle)
{
    __unused(node);
    __unused(handle);

    return PIPE_BUFFER_SIZE;
}

static void pipe_destroy(FsPipe *node)
{
    ringbuffer_destroy(node->_buffer);
}

FsPipe::FsPipe() : FsNode(FILE_TYPE_PIPE)
{
    size = (FsNodeSizeCallback)pipe_size;
    destroy = (FsNodeDestroyCallback)pipe_destroy;
    _buffer = ringbuffer_create(PIPE_BUFFER_SIZE);
}

bool FsPipe::can_read(FsHandle *handle)
{
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(_buffer) || !writers;
}

bool FsPipe::can_write(FsHandle *handle)
{
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_full(_buffer) || !readers;
}

ResultOr<size_t> FsPipe::read(FsHandle &handle, void *buffer, size_t size)
{
    __unused(handle);

    if (!writers)
    {
        return ERR_STREAM_CLOSED;
    }

    return ringbuffer_read(_buffer, (char *)buffer, size);
}

ResultOr<size_t> FsPipe::write(FsHandle &handle, const void *buffer, size_t size)
{
    __unused(handle);

    if (!readers)
    {
        return ERR_STREAM_CLOSED;
    }

    return ringbuffer_write(_buffer, (const char *)buffer, size);
}
