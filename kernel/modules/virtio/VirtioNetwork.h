#pragma once

#include "virtio/VirtioDevice.h"

struct VirtioNetwork : public VirtioDevice
{
private:
public:
    VirtioNetwork(DeviceAddress address) : VirtioDevice(address, DeviceClass::NETWORK)
    {
    }

    ~VirtioNetwork()
    {
    }
};
