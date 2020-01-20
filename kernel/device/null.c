/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include "filesystem/Filesystem.h"

static int null_FsOperationRead(FsNode *node, FsHandle *handle, void *buffer, uint size)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);
    __unused(size);

    return 0;
}

static int null_FsOperationWrite(FsNode *node, FsHandle *handle, const void *buffer, uint size)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);

    return size;
}

void null_initialize(void)
{
    FsNode *null_device = __create(FsNode);

    fsnode_init(null_device, FSNODE_DEVICE);

    FSNODE(null_device)->read = (FsOperationRead)null_FsOperationRead;
    FSNODE(null_device)->write = (FsOperationWrite)null_FsOperationWrite;

    Path *null_device_path = path("/dev/null");
    filesystem_link_and_take_ref(null_device_path, null_device);
    path_delete(null_device_path);
}