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

class RandomDevice : public FsNode
{
private:
    /* data */
public:
    RandomDevice() : FsNode(FILE_TYPE_DEVICE)
    {
        read = (FsNodeReadCallback)random_read;
        write = (FsNodeWriteCallback)random_write;
    }

    ~RandomDevice() {}
};

void random_initialize()
{
    _random = (Random){
        6389,
        6389,
    };

    filesystem_link_and_take_ref_cstring(UNIX_DEVICE_PATH("random"), new RandomDevice());
}
