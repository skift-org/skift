#pragma once

#include <string.h>

#include "kernel/devices/UNIXDevice.h"

class UnixZero : public UNIXDevice
{
private:
public:
    UnixZero(DeviceAddress address) : UNIXDevice(address, DeviceClass::ZERO)
    {
    }

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override
    {
        __unused(handle);

        memset(buffer, 0, size);

        return size;
    }

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size) override
    {
        __unused(handle);
        __unused(buffer);

        return size;
    }
};
