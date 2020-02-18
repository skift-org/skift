/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/MinMax.h>
#include <libsystem/Result.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include "kernel/node/File.h"
#include "kernel/node/Handle.h"

Result file_FsOperationOpen(FsFile *node, FsHandle *handle)
{
    if (fshandle_has_flag(handle, OPEN_TRUNC))
    {
        free(node->buffer);
        node->buffer = malloc(512);
        node->realsize = 512;
        node->size = 0;
    }

    return SUCCESS;
}

Result file_FsOperationRead(FsFile *node, FsHandle *handle, void *buffer, size_t size, size_t *readed)
{
    if (handle->offset <= node->size)
    {
        *readed = MIN(node->size - handle->offset, size);
        memcpy(buffer, (byte *)node->buffer + handle->offset, *readed);
    }

    return SUCCESS;
}

Result file_FsOperationWrite(FsFile *node, FsHandle *handle, const void *buffer, size_t size, size_t *writen)
{
    if ((handle->offset + size) > node->realsize)
    {
        node->buffer = realloc(node->buffer, handle->offset + size);
        node->realsize = handle->offset + size;
    }

    node->size = MAX(handle->offset + size, node->size);
    memcpy((byte *)(node->buffer) + handle->offset, buffer, size);

    *writen = size;

    return SUCCESS;
}

size_t file_FsOperationSize(FsFile *node, FsHandle *handle)
{
    __unused(handle);

    return node->size;
}

void file_FsOperationDestroy(FsFile *node)
{
    free(node->buffer);
}

FsNode *file_create(void)
{
    FsFile *file = __create(FsFile);

    fsnode_init(FSNODE(file), FSNODE_FILE);

    FSNODE(file)->open = (FsOperationOpen)file_FsOperationOpen;
    FSNODE(file)->read = (FsOperationRead)file_FsOperationRead;
    FSNODE(file)->write = (FsOperationWrite)file_FsOperationWrite;
    FSNODE(file)->size = (FsOperationSize)file_FsOperationSize;
    FSNODE(file)->destroy = (FsOperationDestroy)file_FsOperationDestroy;

    file->buffer = malloc(512);
    file->realsize = 512;
    file->size = 0;

    return (FsNode *)file;
}
