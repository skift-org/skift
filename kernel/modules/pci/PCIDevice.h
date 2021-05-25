#pragma once

#include "system/devices/Driver.h"
#include "system/memory/MemoryRange.h"

#include "pci/PCI.h"

enum struct PCIBarType
{
    MMIO32,
    MMIO64,
    PIO,
};

struct PCIBar
{
private:
    PCIBarType _type;
    uint32_t _base;
    size_t _size;
    bool _prefetchable;

public:
    PCIBarType type() { return _type; }
    uint32_t base() { return _base; }
    size_t size() { return _size; }
    bool prefetchable() { return _prefetchable; }

    MemoryRange range()
    {
        assert(type() == PCIBarType::MMIO32 ||
               type() == PCIBarType::MMIO64);

        return MemoryRange{base(), size()};
    }

    PCIBar(PCIBarType type, uint32_t base, size_t size, bool prefetchable)
        : _type(type),
          _base(base),
          _size(size),
          _prefetchable(prefetchable)
    {
    }
};

struct PCIDevice : public Device
{
private:
    int _vendor;
    int _device;
    int _device_class;
    int _device_subclass;
    int _interrupt;

public:
    int bus() { return pci_address().bus(); }

    int slot() { return pci_address().slot(); }

    int func() { return pci_address().func(); }

    PCIAddress pci_address() { return address().pci(); }

    int vendor() { return _vendor; }

    int device() { return _device; }

    int device_class() { return _device_class; }

    int device_subclass() { return _device_subclass; }

    int interrupt() override { return _interrupt; }

    PCIDevice(DeviceAddress address, DeviceClass klass) : Device(address, klass)
    {
        _vendor = pci_address().read16(PCI_VENDOR_ID);
        _device = pci_address().read16(PCI_DEVICE_ID);
        _device_class = pci_address().read8(PCI_CLASS);
        _device_subclass = pci_address().read8(PCI_SUBCLASS);
        _interrupt = pci_get_interrupt(pci_address());
    }

    PCIBar bar(int index)
    {
        assert(index >= 0 && index <= 5);

        size_t bar_offset = PCI_BAR0 + index * 4;

        auto read_bar = [&]() {
            return pci_address().read32(bar_offset);
        };

        auto write_bar = [&](uint32_t value) {
            return pci_address().write32(bar_offset, value);
        };

        uint32_t bar_value = read_bar();

        PCIBarType type;
        uint32_t base;

        size_t size;
        bool prefetchable = false;

        if ((bar_value & 0b0111) == 0b0110)
        {
            type = PCIBarType::MMIO64;
            base = bar_value & 0xFFFFFFF0;
            prefetchable = (bar_value & 0b1000) == 0b1000;
        }
        else if ((bar_value & 0b0111) == 0b0000)
        {
            type = PCIBarType::MMIO32;
            base = bar_value & 0xFFFFFFF0;
            prefetchable = (bar_value & 0b1000) == 0b1000;
        }
        else if ((bar_value & 0b0111) == 0b0001)
        {
            type = PCIBarType::PIO;
            base = bar_value & 0xFFFFFFFC;
        }
        else
        {
            ASSERT_NOT_REACHED();
        }

        write_bar(0xFFFFFFFF);
        uint32_t new_bar_value = read_bar();
        write_bar(bar_value);

        size = ~(new_bar_value & 0xFFFFFFF0) + 1;

        return PCIBar{type, base, size, prefetchable};
    }
};

template <typename PCIDeviceType>
struct PCIDeviceMatcher : public DeviceMatcher
{
private:
    uint16_t _vendor;
    uint16_t _device;

public:
    PCIDeviceMatcher(const char *name, uint16_t vendor, uint16_t device)
        : DeviceMatcher(BUS_PCI, name),
          _vendor(vendor),
          _device(device)
    {
    }

    bool match(DeviceAddress address)
    {
        return _vendor == address.pci().read16(PCI_VENDOR_ID) &&
               _device == address.pci().read16(PCI_DEVICE_ID);
    }

    RefPtr<Device> instance(DeviceAddress address)
    {
        return make<PCIDeviceType>(address);
    }
};
