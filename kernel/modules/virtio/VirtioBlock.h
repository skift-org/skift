#pragma once

#include "virtio/VirtioDevice.h"

struct VirtioBlock : public VirtioDevice
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
