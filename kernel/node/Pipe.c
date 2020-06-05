
#include <libsystem/Result.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Pipe.h"

#define PIPE_BUFFER_SIZE 4096

static bool pipe_can_read(FsPipe *node, FsHandle *handle)
{
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(node->buffer) || !node->node.writers;
}

static bool pipe_can_write(FsPipe *node, FsHandle *handle)
{
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_full(node->buffer) || !node->node.readers;
}

static Result pipe_read(FsPipe *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(handle);

    if (!node->node.writers)
    {
        return ERR_STREAM_CLOSED;
    }

    *read = ringbuffer_read(node->buffer, (char *)buffer, size);

    return SUCCESS;
}

static Result pipe_write(FsPipe *node, FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    __unused(handle);

    if (!node->node.readers)
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

FsNode *fspipe_create(void)
{
    FsPipe *pipe = __create(FsPipe);

    fsnode_init(FSNODE(pipe), FILE_TYPE_PIPE);

    FSNODE(pipe)->can_read = (FsNodeCanReadCallback)pipe_can_read;
    FSNODE(pipe)->can_write = (FsNodeCanWriteCallback)pipe_can_write;
    FSNODE(pipe)->read = (FsNodeReadCallback)pipe_read;
    FSNODE(pipe)->write = (FsNodeWriteCallback)pipe_write;
    FSNODE(pipe)->size = (FsNodeSizeCallback)pipe_size;
    FSNODE(pipe)->destroy = (FsNodeDestroyCallback)pipe_destroy;

    pipe->buffer = ringbuffer_create(PIPE_BUFFER_SIZE);

    return (FsNode *)pipe;
}
