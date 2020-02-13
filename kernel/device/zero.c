/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include "kernel/filesystem/Filesystem.h"

static Result zero_FsOperationRead(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *readed)
{
    __unused(node);
    __unused(handle);

    memset(buffer, 0, size);

    *readed = size;

    return SUCCESS;
}

static Result zero_FsOperationWrite(FsNode *node, FsHandle *handle, const void *buffer, size_t size, size_t *writen)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);

    *writen = size;

    return SUCCESS;
}

void zero_initialize(void)
{
    FsNode *zero_device = __create(FsNode);
    fsnode_init(zero_device, FSNODE_DEVICE);

    FSNODE(zero_device)->read = (FsOperationRead)zero_FsOperationRead;
    FSNODE(zero_device)->write = (FsOperationWrite)zero_FsOperationWrite;

    Path *zero_device_path = path_create("/dev/zero");
    filesystem_link_and_take_ref(zero_device_path, zero_device);
    path_destroy(zero_device_path);
}