#pragma once

#include "kernel/devices/Device.h"
#include "kernel/node/Node.h"

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

    size_t size() override
    {
        return _device->size();
    }

    bool can_read(FsHandle *) override
    {
        return _device->can_read();
    }

    bool can_write(FsHandle *) override
    {
        return _device->can_write();
    }

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override
    {
        return _device->read(handle.offset(), buffer, size);
    }

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size) override
    {
        return _device->write(handle.offset(), buffer, size);
    }

    Result call(FsHandle &, IOCall request, void *args) override
    {
        return _device->call(request, args);
    }
};
