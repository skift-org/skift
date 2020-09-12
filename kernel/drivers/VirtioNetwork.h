#pragma once

#include "kernel/devices/VirtioDevice.h"

class VirtioNetwork : public VirtioDevice
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
