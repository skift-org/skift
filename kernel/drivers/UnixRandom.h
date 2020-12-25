#pragma once

#include <libutils/Random.h>

#include "kernel/devices/UNIXDevice.h"

class UnixRandom : public UNIXDevice
{
private:
    Random _random{};

public:
    UnixRandom(DeviceAddress address) : UNIXDevice(address, DeviceClass::RANDOM)
    {
    }

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override
    {
        __unused(handle);

        for (size_t i = 0; i < size; i++)
        {
            ((char *)buffer)[i] = _random.next_u8();
        }

        return size;
    }

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size) override
    {
        __unused(handle);
        __unused(buffer);

        return size;
    }
};
