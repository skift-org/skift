#include "kernel/bus/PCIDevice.h"
#include "kernel/bus/PCI.h"
#include "kernel/x86/x86.h"

void pci_device_write(PCIDevice device, int field, int size, uint32_t value)
{
    __unused(size);

    out32(PCI_ADDRESS_PORT, pci_device_get_address(device, field));
    out32(PCI_VALUE_PORT, value);
}

uint32_t pci_device_read(PCIDevice device, int field, int size)
{
    out32(PCI_ADDRESS_PORT, pci_device_get_address(device, field));

    if (size == 4)
    {
        uint32_t t = in32(PCI_VALUE_PORT);
        return t;
    }
    else if (size == 2)
    {
        uint16_t t = in16(PCI_VALUE_PORT + (field & 2));
        return t;
    }
    else if (size == 1)
    {
        uint8_t t = in8(PCI_VALUE_PORT + (field & 3));
        return t;
    }
    return 0xFFFF;
}

uint16_t pci_device_type(PCIDevice device)
{
    return (pci_device_read(device, PCI_CLASS, 1) << 8) |
           pci_device_read(device, PCI_SUBCLASS, 1);
}