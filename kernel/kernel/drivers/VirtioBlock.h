#pragma once

#include "kernel/devices/VirtioDevice.h"

class VirtioBlock : public VirtioDevice
{
private:
public:
    VirtioBlock(DeviceAddress address) : VirtioDevice(address, DeviceClass::DISK)
    {
    }

    ~VirtioBlock()
    {
    }
};
