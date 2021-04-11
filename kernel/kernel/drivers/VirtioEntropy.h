#pragma once

#include "kernel/devices/VirtioDevice.h"

class VirtioEntropy : public VirtioDevice
{
private:
public:
    VirtioEntropy(DeviceAddress address) : VirtioDevice(address, DeviceClass::RANDOM)
    {
    }

    ~VirtioEntropy()
    {
    }
};
