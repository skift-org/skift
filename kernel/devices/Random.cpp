#include <abi/Paths.h>
#include <libsystem/system/Random.h>

#include "kernel/filesystem/Filesystem.h"

static Random _random;

static Result random_read(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);
    __unused(handle);

    for (size_t i = 0; i < size; i++)
    {
        ((char *)buffer)[i] = random_uint32_max(&_random, 255);
    }

    *read = size;

    return SUCCESS;
}

static Result random_write(FsNode *node, FsHandle *handle, const void *buffer, uint size, size_t *written)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);

    *written = size;

    return SUCCESS;
}

void random_initialize()
{
    _random = (Random){
        6389,
        6389,
    };

    FsNode *random_device = __create(FsNode);

    fsnode_init(random_device, FILE_TYPE_DEVICE);

    random_device->read = (FsNodeReadCallback)random_read;
    random_device->write = (FsNodeWriteCallback)random_write;

    Path *random_device_path = path_create(UNIX_DEVICE_PATH("random"));
    filesystem_link_and_take_ref(random_device_path, random_device);
    path_destroy(random_device_path);
}
