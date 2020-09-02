#include <libsystem/Assert.h>

#include "arch/x86/x86.h"
#include "kernel/bus/PCI.h"
#include "kernel/bus/PCIDevice.h"

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
        return in32(PCI_VALUE_PORT);
    }
    else if (size == 2)
    {
        return in16(PCI_VALUE_PORT + (field & 2));
    }
    else if (size == 1)
    {
        return in8(PCI_VALUE_PORT + (field & 3));
    }

    return 0xFFFF;
}

uint32_t pci_device_read_bar(PCIDevice device, int bar)
{
    assert(bar >= 0 && bar <= 5);

    int bar_field = PCI_BAR0 + bar * 4;

    return pci_device_read(device, bar_field, 4);
}

size_t pci_device_size_bar(PCIDevice device, int bar)
{
    assert(bar >= 0 && bar <= 5);

    uint8_t field = (PCI_BAR0 + (bar << 2));
    uint32_t bar_reserved = pci_device_read(device, field, 4);
    pci_device_write(device, field, 4, 0xFFFFFFFF);

    uint32_t space_size = pci_device_read(device, field, 4);
    pci_device_write(device, field, 4, bar_reserved);

    space_size &= 0xfffffff0;
    space_size = (~space_size) + 1;

    return space_size;
}

PCIBarType pci_device_type_bar(PCIDevice device, int bar)
{
    assert(bar >= 0 && bar <= 5);

    uint32_t bar_value = pci_device_read_bar(device, bar);

    if (bar_value & 0b0001)
    {
        return PCIBarType::PIO;
    }
    else
    {
        if ((bar_value & 0b0110) == 0b0110)
        {
            return PCIBarType::MMIO64;
        }
        else
        {
            return PCIBarType::MMIO32;
        }
    }
}

void pci_device_write_bar(PCIDevice device, int bar, uint32_t value)
{
    assert(bar >= 0 && bar <= 5);

    int bar_field = PCI_BAR0 + bar * 4;

    pci_device_write(device, bar_field, 4, value);
}

uint16_t pci_device_type(PCIDevice device)
{
    return (pci_device_read(device, PCI_CLASS, 1) << 8) |
           pci_device_read(device, PCI_SUBCLASS, 1);
}
