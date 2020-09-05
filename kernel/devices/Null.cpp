#include <abi/Paths.h>
#include <libsystem/core/CString.h>

#include "kernel/filesystem/Filesystem.h"

class Null : public FsNode
{
private:
public:
    Null() : FsNode(FILE_TYPE_DEVICE)
    {
    }

    ~Null() {}

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size)
    {
        __unused(handle);
        __unused(buffer);
        __unused(size);

        return 0;
    }

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size)
    {
        __unused(handle);
        __unused(buffer);
        __unused(size);

        return size;
    }
};

void null_initialize()
{
    filesystem_link_and_take_ref_cstring(UNIX_DEVICE_PATH("null"), new Null());
}
