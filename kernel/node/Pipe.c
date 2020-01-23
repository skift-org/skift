/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/error.h>

#include "node/Handle.h"
#include "node/Pipe.h"

bool pipe_FsOperationCanRead(FsPipe *node)
{
    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(node->buffer);
}

bool pipe_FsOperationCanWrite(FsPipe *node)
{
    // FIXME: make this atomic or something...
    return !ringbuffer_is_full(node->buffer);
}

int pipe_FsOperationRead(FsPipe *node, FsHandle *handle, void *buffer, uint size)
{
    __unused(handle);

    return ringbuffer_read(node->buffer, buffer, size);
}

int pipe_FsOperationWrite(FsPipe *node, FsHandle *handle, const void *buffer, uint size)
{
    __unused(handle);

    return ringbuffer_write(node->buffer, buffer, size);
}

size_t pipe_FsOperationSize(FsPipe *node, FsHandle *handle)
{
    __unused(node);
    __unused(handle);

    return 512;
}

void pipe_FsOperationDestroy(FsPipe *node)
{
    ringbuffer_destroy(node->buffer);
}

FsNode *pipe_create(void)
{
    FsPipe *pipe = __create(FsPipe);

    fsnode_init(FSNODE(pipe), FSNODE_PIPE);

    FSNODE(pipe)->can_read = (FsOperationCanRead)pipe_FsOperationCanRead;
    FSNODE(pipe)->can_write = (FsOperationCanWrite)pipe_FsOperationCanWrite;
    FSNODE(pipe)->read = (FsOperationRead)pipe_FsOperationRead;
    FSNODE(pipe)->write = (FsOperationWrite)pipe_FsOperationWrite;
    FSNODE(pipe)->size = (FsOperationSize)pipe_FsOperationSize;
    FSNODE(pipe)->destroy = (FsOperationDestroy)pipe_FsOperationDestroy;

    pipe->buffer = ringbuffer_create(512);

    return (FsNode *)pipe;
}
