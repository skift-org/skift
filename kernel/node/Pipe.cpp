
#include <libsystem/Result.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Pipe.h"

#define PIPE_BUFFER_SIZE 4096

static Result pipe_read(FsPipe *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(handle);

    if (!node->writers)
    {
        return ERR_STREAM_CLOSED;
    }

    *read = ringbuffer_read(node->buffer, (char *)buffer, size);

    return SUCCESS;
}

static Result pipe_write(FsPipe *node, FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    __unused(handle);

    if (!node->readers)
    {
        return ERR_STREAM_CLOSED;
    }

    *written = ringbuffer_write(node->buffer, (const char *)buffer, size);

    return SUCCESS;
}

static size_t pipe_size(FsPipe *node, FsHandle *handle)
{
    __unused(node);
    __unused(handle);

    return PIPE_BUFFER_SIZE;
}

static void pipe_destroy(FsPipe *node)
{
    ringbuffer_destroy(node->buffer);
}

FsPipe::FsPipe() : FsNode(FILE_TYPE_PIPE)
{
    read = (FsNodeReadCallback)pipe_read;
    write = (FsNodeWriteCallback)pipe_write;
    size = (FsNodeSizeCallback)pipe_size;
    destroy = (FsNodeDestroyCallback)pipe_destroy;

    buffer = ringbuffer_create(PIPE_BUFFER_SIZE);
}

bool FsPipe::can_read(FsHandle *handle)
{
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(buffer) || !writers;
}

bool FsPipe::can_write(FsHandle *handle)
{
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_full(buffer) || !readers;
}