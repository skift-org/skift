/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/math.h>
#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libsystem/logger.h>

#include "node/File.h"
#include "node/Handle.h"

int file_FsOperationOpen(FsFile *node, Handle *handle)
{
    if (handle_has_flag(handle, OPEN_TRUNC))
    {
        free(node->buffer);
        node->buffer = malloc(512);
        node->realsize = 512;
        node->size = 0;
    }

    return -ERR_SUCCESS;
}

int file_FsOperationRead(FsFile *node, Handle *handle, void *buffer, size_t size)
{
    size_t result = 0;

    if (handle->offset <= node->size)
    {
        int readedsize = min(node->size - handle->offset, size);
        memcpy(buffer, (byte *)node->buffer + handle->offset, readedsize);

        result = readedsize;
    }

    return result;
}

int file_FsOperationWrite(FsFile *node, Handle *handle, const void *buffer, size_t size)
{
    int result = 0;

    if ((handle->offset + size) > node->realsize)
    {
        node->buffer = realloc(node->buffer, handle->offset + size);
        node->realsize = handle->offset + size;
    }

    node->size = max(handle->offset + size, node->size);
    memcpy((byte *)(node->buffer) + handle->offset, buffer, size);

    result = size;

    return result;
}

size_t file_FsOperationSize(FsFile *node, Handle *handle)
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
