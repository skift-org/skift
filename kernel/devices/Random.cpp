#include <abi/Paths.h>
#include <libsystem/system/Random.h>

#include "kernel/filesystem/Filesystem.h"

class RandomDevice : public FsNode
{
private:
    Random _random;

public:
    RandomDevice() : FsNode(FILE_TYPE_DEVICE)
    {
        _random = (Random){
            6389,
            6389,
        };
    }

    ~RandomDevice() {}

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size)
    {
        __unused(handle);

        for (size_t i = 0; i < size; i++)
        {
            ((char *)buffer)[i] = random_uint32_max(&_random, 255);
        }

        return size;
    }

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size)
    {
        __unused(handle);
        __unused(buffer);

        return size;
    }
};

void random_initialize()
{
    filesystem_link_and_take_ref_cstring(UNIX_DEVICE_PATH("random"), new RandomDevice());
}
