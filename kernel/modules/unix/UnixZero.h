#pragma once

#include <string.h>

#include "unix/UNIXDevice.h"

struct UnixZero : public UNIXDevice
{
private:
public:
    UnixZero(DeviceAddress address) : UNIXDevice(address, DeviceClass::ZERO)
    {
    }

    ResultOr<size_t> read(size64_t offset, void *buffer, size_t size) override
    {
        UNUSED(offset);

        memset(buffer, 0, size);

        return size;
    }

    ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size) override
    {
        UNUSED(offset);
        UNUSED(buffer);

        return size;
    }
};
