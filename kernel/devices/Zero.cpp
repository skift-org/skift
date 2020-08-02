#include <abi/Paths.h>
#include <libsystem/core/CString.h>

#include "kernel/filesystem/Filesystem.h"

static Result zero_read(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);
    __unused(handle);

    memset(buffer, 0, size);

    *read = size;

    return SUCCESS;
}

static Result zero_write(FsNode *node, FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);

    *written = size;

    return SUCCESS;
}

void zero_initialize()
{
    FsNode *zero_device = __create(FsNode);
    fsnode_init(zero_device, FILE_TYPE_DEVICE);

    zero_device->read = (FsNodeReadCallback)zero_read;
    zero_device->write = (FsNodeWriteCallback)zero_write;

    filesystem_link_and_take_ref_cstring(UNIX_DEVICE_PATH("zero"), zero_device);
}
