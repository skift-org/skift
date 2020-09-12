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
class VirtioDeviceDriver : public DeviceDriver
{
private:
    VirtioAddress _address;

public:
    VirtioDeviceDriver(const char *name, VirtioAddress address)
        : DeviceDriver(BUS_PCI, name), _address(address)
    {
    }

    bool match(DeviceAddress address)
    {
        uint16_t vendor = address.pci().read16(PCI_VENDOR_ID);
        uint16_t device = address.pci().read16(PCI_DEVICE_ID);
        uint16_t subsystem = address.pci().read16(PCI_SUBSYSTEM_ID);

        return vendor == 0x1AF4 &&
               device >= 0x1000 && device < 0x1040 &&
               subsystem == _address;
    }

    RefPtr<Device> instance(DeviceAddress address)
    {
        return make<VirtioDeviceType>(address);
    }
};
