#pragma once

#include "unix/UNIXDevice.h"

class UnixNull : public UNIXDevice
{
private:
public:
    UnixNull(DeviceAddress address) : UNIXDevice(address, DeviceClass::NULL_)
    {
    }

    ~UnixNull() {}

    ResultOr<size_t> read(size64_t offset, void *buffer, size_t size) override
    {
        UNUSED(offset);
        UNUSED(buffer);
        UNUSED(size);

        return 0;
    }

    ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size) override
    {
        UNUSED(offset);
        UNUSED(buffer);
        UNUSED(size);

        return size;
    }
};
