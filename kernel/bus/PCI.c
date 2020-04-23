/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include "kernel/bus/PCI.h"
#include "kernel/x86/x86.h"

void pci_write_field(uint32_t device, int field, int size, uint32_t value)
{
    __unused(size);

    out32(PCI_ADDRESS_PORT, pci_get_addr(device, field));
    out32(PCI_VALUE_PORT, value);
}

uint32_t pci_read_field(uint32_t device, int field, int size)
{
    out32(PCI_ADDRESS_PORT, pci_get_addr(device, field));

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

uint16_t pci_read_type(uint32_t dev)
{
    return (pci_read_field(dev, PCI_CLASS, 1) << 8) | pci_read_field(dev, PCI_SUBCLASS, 1);
}

IterationDecision pci_device_iterate_hit(
    void *target,
    DeviceIterateCallback callback,
    uint32_t device)
{
    int vendor_id = (int)pci_read_field(device, PCI_VENDOR_ID, 2);
    int device_id = (int)pci_read_field(device, PCI_DEVICE_ID, 2);

    return callback(
        target,
        (DeviceInfo){
            .bus = BUS_PCI,
            .device = device,
            .vendor_id = vendor_id,
            .device_id = device_id,
        });
}

IterationDecision pci_device_iterate_func(
    void *target,
    DeviceIterateCallback callback,
    int bus,
    int slot,
    int func)
{
    uint32_t device = pci_box_device(bus, slot, func);
    if (pci_device_iterate_hit(target, callback, device) == ITERATION_STOP)
    {
        return ITERATION_STOP;
    }

    if (pci_read_type(device) == PCI_TYPE_BRIDGE)
    {
        return pci_device_iterate_bus(
            target,
            callback,
            pci_read_field(device, PCI_SECONDARY_BUS, 1));
    }
    else
    {
        return ITERATION_CONTINUE;
    }
}

IterationDecision pci_device_iterate_slot(
    void *target,
    DeviceIterateCallback callback,
    int bus,
    int slot)
{
    uint32_t dev = pci_box_device(bus, slot, 0);
    if (pci_read_field(dev, PCI_VENDOR_ID, 2) == PCI_NONE)
        return ITERATION_CONTINUE;

    if (pci_device_iterate_func(target, callback, bus, slot, 0) == ITERATION_STOP)
        return ITERATION_STOP;

    if (!pci_read_field(dev, PCI_HEADER_TYPE, 1))
        return ITERATION_CONTINUE;

    for (int func = 1; func < 8; func++)
    {
        uint32_t dev = pci_box_device(bus, slot, func);

        if (pci_read_field(dev, PCI_VENDOR_ID, 2) != PCI_NONE)
            if (pci_device_iterate_func(target, callback, bus, slot, func) == ITERATION_STOP)
                return ITERATION_STOP;
    }

    return ITERATION_CONTINUE;
}

IterationDecision pci_device_iterate_bus(void *target, DeviceIterateCallback callback, int bus)
{
    for (int slot = 0; slot < 32; ++slot)
        if (pci_device_iterate_slot(target, callback, bus, slot) == ITERATION_STOP)
            return ITERATION_STOP;

    return ITERATION_CONTINUE;
}

IterationDecision pci_device_iterate(void *target, DeviceIterateCallback callback)
{
    if ((pci_read_field(0, PCI_HEADER_TYPE, 1) & 0x80) == 0)
    {
        return pci_device_iterate_bus(target, callback, 0);
    }

    for (int func = 0; func < 8; ++func)
    {
        uint32_t dev = pci_box_device(0, 0, func);

        if (pci_read_field(dev, PCI_VENDOR_ID, 2) == PCI_NONE)
        {
            return ITERATION_CONTINUE;
        }

        if (pci_device_iterate_bus(target, callback, func) == ITERATION_STOP)
        {
            return ITERATION_STOP;
        }
    }

    return ITERATION_CONTINUE;
}

static IterationDecision find_isa_bridge(uint32_t *pci_isa, DeviceInfo info)
{
    if (info.vendor_id != 0x8086)
        return ITERATION_CONTINUE;

    if (info.device_id != 0x7000 ||
        info.device_id != 0x7110)
        return ITERATION_CONTINUE;

    *pci_isa = info.device;

    return ITERATION_STOP;
}

static uint32_t pci_isa = 0;
static uint8_t pci_remaps[4] = {};

void pci_remap(void)
{
    pci_device_iterate(&pci_isa, (DeviceIterateCallback)find_isa_bridge);

    if (pci_isa)
    {
        for (int i = 0; i < 4; ++i)
        {
            pci_remaps[i] = pci_read_field(pci_isa, 0x60 + i, 1);
            if (pci_remaps[i] == 0x80)
            {
                pci_remaps[i] = 10 + (i % 1);
            }
        }
        uint32_t out = 0;
        memcpy(&out, &pci_remaps, 4);
        pci_write_field(pci_isa, 0x60, 4, out);
    }
}

int pci_get_interrupt(uint32_t device)
{
    if (pci_isa)
    {
        uint32_t irq_pin = pci_read_field(device, 0x3D, 1);
        if (irq_pin == 0)
        {
            logger_error("PCI device does not specific interrupt line");
            return pci_read_field(device, PCI_INTERRUPT_LINE, 1);
        }

        int pirq = (irq_pin + pci_extract_slot(device) - 2) % 4;
        int int_line = pci_read_field(device, PCI_INTERRUPT_LINE, 1);

        logger_error("slot is %d, irq pin is %d, so pirq is %d and that maps to %d? int_line=%d", pci_extract_slot(device), irq_pin, pirq, pci_remaps[pirq], int_line);

        for (int i = 0; i < 4; ++i)
        {
            logger_error("  irq[%d] = %d", i, pci_remaps[i]);
        }

        if (pci_remaps[pirq] >= 0x80)
        {
            logger_error("not mapped, remapping?");
            if (int_line == 0xFF)
            {
                int_line = 10;
                pci_write_field(device, PCI_INTERRUPT_LINE, 1, int_line);
                logger_error("? Just going in blind here.\n");
            }
            pci_remaps[pirq] = int_line;
            uint32_t out = 0;
            memcpy(&out, &pci_remaps, 4);
            pci_write_field(pci_isa, 0x60, 4, out);
            return int_line;
        }
        pci_write_field(device, PCI_INTERRUPT_LINE, 1, pci_remaps[pirq]);
        return pci_remaps[pirq];
    }
    else
    {
        return pci_read_field(device, PCI_INTERRUPT_LINE, 1);
    }
}
