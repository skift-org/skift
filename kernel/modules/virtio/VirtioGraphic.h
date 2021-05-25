#pragma once

#include "virtio/VirtioDevice.h"

struct VirtioGraphic : public VirtioDevice
{
private:
public:
    VirtioGraphic(DeviceAddress address) : VirtioDevice(address, DeviceClass::FRAMEBUFFER)
    {
    }

    ~VirtioGraphic()
    {
    }
};
