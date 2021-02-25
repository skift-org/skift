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
