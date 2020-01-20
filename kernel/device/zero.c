/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include "filesystem/Filesystem.h"

static int zero_FsOperationRead(FsNode *node, FsHandle *handle, void *buffer, uint size)
{
    __unused(node);
    __unused(handle);

    memset(buffer, 0, size);

    return size;
}

static int zero_FsOperationWrite(FsNode *node, FsHandle *handle, const void *buffer, uint size)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);

    return size;
}

void zero_initialize(void)
{
    FsNode *zero_device = __create(FsNode);
    fsnode_init(zero_device, FSNODE_DEVICE);

    FSNODE(zero_device)->read = (FsOperationRead)zero_FsOperationRead;
    FSNODE(zero_device)->write = (FsOperationWrite)zero_FsOperationWrite;

    Path *zero_device_path = path("/dev/zero");
    filesystem_link_and_take_ref(zero_device_path, zero_device);
    path_delete(zero_device_path);
}