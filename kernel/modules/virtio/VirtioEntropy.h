#pragma once

#include "virtio/VirtioDevice.h"

struct VirtioEntropy : public VirtioDevice
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
