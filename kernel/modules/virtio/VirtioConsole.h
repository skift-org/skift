#pragma once

#include "virtio/VirtioDevice.h"

struct VirtioConsole : public VirtioDevice
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
