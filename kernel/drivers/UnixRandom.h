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

    ResultOr<size_t> read(size64_t offset, void *buffer, size_t size) override
    {
        __unused(offset);

        for (size_t i = 0; i < size; i++)
        {
            ((char *)buffer)[i] = _random.next_u8();
        }

        return size;
    }

    ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size) override
    {
        __unused(offset);
        __unused(buffer);

        return size;
    }
};
