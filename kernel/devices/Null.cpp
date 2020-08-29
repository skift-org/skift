#include <abi/Paths.h>
#include <libsystem/core/CString.h>

#include "kernel/filesystem/Filesystem.h"

static Result null_read(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);
    __unused(size);

    *read = 0;

    return SUCCESS;
}

static Result null_write(FsNode *node, FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);

    *written = size;

    return SUCCESS;
}

class Null : public FsNode
{
private:
    /* data */
public:
    Null() : FsNode(FILE_TYPE_DEVICE)
    {
        read = (FsNodeReadCallback)null_read;
        write = (FsNodeWriteCallback)null_write;
    }

    ~Null() {}
};

void null_initialize()
{
    filesystem_link_and_take_ref_cstring(UNIX_DEVICE_PATH("null"), new Null());
}
