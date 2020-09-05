#include <abi/Paths.h>
#include <libsystem/core/CString.h>

#include "kernel/filesystem/Filesystem.h"

class Zero : public FsNode
{
private:
public:
    Zero() : FsNode(FILE_TYPE_DEVICE)
    {
    }

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size)
    {
        __unused(handle);

        memset(buffer, 0, size);

        return size;
    }

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size)
    {
        __unused(handle);
        __unused(buffer);

        return size;
    }
};

void zero_initialize()
{
    filesystem_link_and_take_ref_cstring(UNIX_DEVICE_PATH("zero"), new Zero());
}
