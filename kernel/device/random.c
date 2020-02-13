/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/filesystem/Filesystem.h"

static int random_state = 1411743402;

static Result random_FsOperationRead(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *readed)
{
    __unused(node);
    __unused(handle);

    byte *b = buffer;

    for (uint i = 0; i < size; i++)
    {
        uint x = random_state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        random_state = x;

        b[i] = (byte)x;
    }

    *readed = size;

    return SUCCESS;
}

static Result random_FsOperationWrite(FsNode *node, FsHandle *handle, const void *buffer, uint size, size_t *writen)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);

    *writen = size;

    return SUCCESS;
}

void random_initialize(void)
{
    FsNode *random_device = __create(FsNode);

    fsnode_init(random_device, FSNODE_DEVICE);

    FSNODE(random_device)->read = (FsOperationRead)random_FsOperationRead;
    FSNODE(random_device)->write = (FsOperationWrite)random_FsOperationWrite;

    Path *random_device_path = path_create("/dev/random");
    filesystem_link_and_take_ref(random_device_path, random_device);
    path_destroy(random_device_path);
}