#pragma once

#include "kernel/devices/VirtioDevice.h"

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
