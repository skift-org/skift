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

class Zero : public FsNode
{
private:
public:
    Zero() : FsNode(FILE_TYPE_DEVICE)
    {
        read = (FsNodeReadCallback)zero_read;
        write = (FsNodeWriteCallback)zero_write;
    }
};

void zero_initialize()
{
    filesystem_link_and_take_ref_cstring(UNIX_DEVICE_PATH("zero"), new Zero());
}
