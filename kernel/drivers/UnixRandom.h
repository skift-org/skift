#pragma once

#include <libsystem/system/Random.h>

#include "kernel/devices/UNIXDevice.h"

class UnixRandom : public UNIXDevice
{
private:
    Random _random;

public:
    UnixRandom(DeviceAddress address) : UNIXDevice(address, DeviceClass::RANDOM)
    {
        _random = (Random){
            6389,
            6389,
        };
    }

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override
    {
        __unused(handle);

        for (size_t i = 0; i < size; i++)
        {
            ((char *)buffer)[i] = random_uint32_max(&_random, 255);
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
