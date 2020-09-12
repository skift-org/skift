#pragma once

#include "kernel/devices/VirtioDevice.h"

class VirtioGraphic : public VirtioDevice
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
