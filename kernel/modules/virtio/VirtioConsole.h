#pragma once

#include "virtio/VirtioDevice.h"

class VirtioConsole : public VirtioDevice
{
private:
public:
    VirtioConsole(DeviceAddress address) : VirtioDevice(address, DeviceClass::CONSOLE)
    {
    }

    ~VirtioConsole()
    {
    }
};
