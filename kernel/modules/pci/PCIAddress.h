#pragma once

#include "archs/x86_32/x86_32.h"

#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION_ID 0x08
#define PCI_SUBSYSTEM_ID 0x2E

#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS 0x0a
#define PCI_CLASS 0x0b
#define PCI_CACHE_LINE_SIZE 0x0c
#define PCI_LATENCY_TIMER 0x0d
#define PCI_HEADER_TYPE 0x0e
#define PCI_BIST 0x0f

#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24

#define PCI_INTERRUPT_LINE 0x3C
#define PCI_INTERRUPT_PIN 0x3D

#define PCI_SECONDARY_BUS 0x19

#define PCI_HEADER_TYPE_DEVICE 0
#define PCI_HEADER_TYPE_BRIDGE 1
#define PCI_HEADER_TYPE_CARDBUS 2

#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_SATA 0x0106

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_VALUE_PORT 0xCFC

#define PCI_NONE 0xFFFF

class PCIAddress
{
private:
    int _bus = 0;
    int _slot = 0;
    int _func = 0;

    void select(size_t offset)
    {
        uint32_t address = 0x80000000 | (bus() << 16) | (slot() << 11) | (func() << 8) | ((offset)&0xFC);
        out32(PCI_ADDRESS_PORT, address);
    }

public:
    int bus() { return _bus; }
    int slot() { return _slot; }
    int func() { return _func; }

    constexpr PCIAddress() {}

    constexpr PCIAddress(int bus, int slot, int func)
        : _bus(bus), _slot(slot), _func(func)
    {
    }

    uint8_t read8(size_t offset)
    {
        select(offset);
        return in8(PCI_VALUE_PORT + (offset & 3));
    }

    uint16_t read16(size_t offset)
    {
        select(offset);
        return in16(PCI_VALUE_PORT + (offset & 2));
    }

    uint32_t read32(size_t offset)
    {
        select(offset);
        return in32(PCI_VALUE_PORT);
    }

    void write8(size_t offset, uint8_t value)
    {
        select(offset);
        out8(PCI_VALUE_PORT, value);
    }

    void write16(size_t offset, uint16_t value)
    {
        select(offset);
        out16(PCI_VALUE_PORT, value);
    }

    void write32(size_t offset, uint32_t value)
    {
        select(offset);
        out32(PCI_VALUE_PORT, value);
    }

    uint16_t read_class_sub_class()
    {
        return (read8(PCI_CLASS) << 8) | read8(PCI_SUBCLASS);
    }
};
