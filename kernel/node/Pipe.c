/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Pipe.h"

#define PIPE_BUFFER_SIZE 4096

bool pipe_FsOperationCanRead(FsPipe *node, FsHandle *handle)
{
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(node->buffer);
}

bool pipe_FsOperationCanWrite(FsPipe *node, FsHandle *handle)
{
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_full(node->buffer);
}

Result pipe_FsOperationRead(FsPipe *node, FsHandle *handle, void *buffer, size_t size, size_t *readed)
{
    __unused(handle);

    *readed = ringbuffer_read(node->buffer, buffer, size);

    return SUCCESS;
}

Result pipe_FsOperationWrite(FsPipe *node, FsHandle *handle, const void *buffer, size_t size, size_t *writen)
{
    __unused(handle);

    *writen = ringbuffer_write(node->buffer, buffer, size);

    return SUCCESS;
}

size_t pipe_FsOperationSize(FsPipe *node, FsHandle *handle)
{
    __unused(node);
    __unused(handle);

    return PIPE_BUFFER_SIZE;
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

    pipe->buffer = ringbuffer_create(PIPE_BUFFER_SIZE);

    return (FsNode *)pipe;
}
