
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/Logger.h>

#include "kernel/node/File.h"
#include "kernel/node/Handle.h"

static Result file_open(FsFile *node, FsHandle *handle)
{
    if (fshandle_has_flag(handle, OPEN_TRUNC))
    {
        free(node->buffer);
        node->buffer = (char *)malloc(512);
        node->realsize = 512;
        node->size = 0;
    }

    return SUCCESS;
}

static Result file_read(FsFile *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    if (handle->offset <= node->size)
    {
        *read = MIN(node->size - handle->offset, size);
        memcpy(buffer, (char *)node->buffer + handle->offset, *read);
    }

    return SUCCESS;
}

static Result file_write(FsFile *node, FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    if ((handle->offset + size) > node->realsize)
    {
        node->buffer = (char *)realloc(node->buffer, handle->offset + size);
        node->realsize = handle->offset + size;
    }

    node->size = MAX(handle->offset + size, node->size);
    memcpy((char *)(node->buffer) + handle->offset, buffer, size);

    *written = size;

    return SUCCESS;
}

static size_t file_size(FsFile *node, FsHandle *handle)
{
    __unused(handle);

    return node->size;
}

static void file_destroy(FsFile *node)
{
    free(node->buffer);
}

FsNode *file_create(void)
{
    FsFile *file = __create(FsFile);

    fsnode_init(FSNODE(file), FILE_TYPE_REGULAR);

    FSNODE(file)->open = (FsNodeOpenCallback)file_open;
    FSNODE(file)->read = (FsNodeReadCallback)file_read;
    FSNODE(file)->write = (FsNodeWriteCallback)file_write;
    FSNODE(file)->size = (FsNodeSizeCallback)file_size;
    FSNODE(file)->destroy = (FsNodeDestroyCallback)file_destroy;

    file->buffer = (char *)malloc(512);
    file->realsize = 512;
    file->size = 0;

    return (FsNode *)file;
}
