
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>

#include "kernel/node/File.h"
#include "kernel/node/Handle.h"

static Result file_open(FsFile *node, FsHandle *handle)
{
    if (fshandle_has_flag(handle, OPEN_TRUNC))
    {
        free(node->buffer);
        node->buffer = (char *)malloc(512);
        node->buffer_allocated = 512;
        node->buffer_size = 0;
    }

    return SUCCESS;
}

static Result file_read(FsFile *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    if (handle->offset <= node->buffer_size)
    {
        *read = MIN(node->buffer_size - handle->offset, size);
        memcpy(buffer, (char *)node->buffer + handle->offset, *read);
    }

    return SUCCESS;
}

static Result file_write(FsFile *node, FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    if ((handle->offset + size) > node->buffer_allocated)
    {
        node->buffer = (char *)realloc(node->buffer, handle->offset + size);
        node->buffer_allocated = handle->offset + size;
    }

    node->buffer_size = MAX(handle->offset + size, node->buffer_size);
    memcpy((char *)(node->buffer) + handle->offset, buffer, size);

    *written = size;

    return SUCCESS;
}

static size_t file_size(FsFile *node, FsHandle *handle)
{
    __unused(handle);

    return node->buffer_size;
}

static void file_destroy(FsFile *node)
{
    free(node->buffer);
}

FsNode *file_create()
{
    FsFile *file = __create(FsFile);

    fsnode_init(file, FILE_TYPE_REGULAR);

    file->open = (FsNodeOpenCallback)file_open;
    file->read = (FsNodeReadCallback)file_read;
    file->write = (FsNodeWriteCallback)file_write;
    file->size = (FsNodeSizeCallback)file_size;
    file->destroy = (FsNodeDestroyCallback)file_destroy;

    file->buffer = (char *)malloc(512);
    file->buffer_allocated = 512;
    file->buffer_size = 0;

    return (FsNode *)file;
}
