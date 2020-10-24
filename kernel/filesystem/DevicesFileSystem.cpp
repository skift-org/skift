#include <abi/Paths.h>
#include <libsystem/Logger.h>

#include "kernel/devices/Device.h"
#include "kernel/devices/Devices.h"
#include "kernel/filesystem/DevicesFileSystem.h"
#include "kernel/filesystem/Filesystem.h"

class FsDevice : public FsNode
{
private:
    RefPtr<Device> _device;

public:
    FsDevice(RefPtr<Device> device)
        : FsNode(FILE_TYPE_DEVICE),
          _device(device)
    {
    }

    ~FsDevice()
    {
    }

    bool can_read(FsHandle *handle) override
    {
        return _device->can_read(*handle);
    }

    bool can_write(FsHandle *handle) override
    {
        return _device->can_write(*handle);
    }

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override
    {
        return _device->read(handle, buffer, size);
    }

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size) override
    {
        return _device->write(handle, buffer, size);
    }

    Result call(FsHandle &handle, IOCall request, void *args) override
    {
        return _device->call(handle, request, args);
    }
};

void devices_filesystem_initialize()
{
    filesystem_mkdir(Path::parse(DEVICE_PATH));

    device_iterate([](auto device) {
        String path = device->path();
        logger_info("Mounting %s to %s", device->address().as_static_cstring(), path.cstring());
        filesystem_link(Path::parse(path), make<FsDevice>(device));

        return Iteration::CONTINUE;
    });
}
