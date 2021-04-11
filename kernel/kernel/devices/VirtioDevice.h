#pragma once

#include "kernel/bus/Virtio.h"
#include "kernel/devices/PCIDevice.h"

class VirtioDevice : public PCIDevice
{
private:
public:
    VirtioDevice(DeviceAddress address, DeviceClass klass) : PCIDevice(address, klass)
    {
    }

    ~VirtioDevice()
    {
    }
};

template <typename VirtioDeviceType>
class VirtioDeviceMatcher : public DeviceMatcher
{
private:
    VirtioAddress _address;

public:
    VirtioDeviceMatcher(const char *name, VirtioAddress address)
        : DeviceMatcher(BUS_PCI, name), _address(address)
    {
    }

    bool match(DeviceAddress address)
    {
        if (address.pci().read16(PCI_VENDOR_ID) != 0x1AF4)
        {
            return false;
        }

        uint16_t device = address.pci().read16(PCI_DEVICE_ID);

        if (device >= 0x1000 && device < 0x1040)
        {
            uint16_t subsystem = address.pci().read16(PCI_SUBSYSTEM_ID);
            return subsystem == _address;
        }
        else
        {
            return (device - 0x1040) == _address;
        }
    }

    RefPtr<Device> instance(DeviceAddress address)
    {
        return make<VirtioDeviceType>(address);
    }
};
