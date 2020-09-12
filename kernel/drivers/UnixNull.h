#pragma once

#include "kernel/devices/UNIXDevice.h"

class UnixNull : public UNIXDevice
{
private:
public:
    UnixNull(DeviceAddress address) : UNIXDevice(address, DeviceClass::NULL_)
    {
    }

    ~UnixNull() {}

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override
    {
        __unused(handle);
        __unused(buffer);
        __unused(size);

        return 0;
    }

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size) override
    {
        __unused(handle);
        __unused(buffer);
        __unused(size);

        return size;
    }
};
