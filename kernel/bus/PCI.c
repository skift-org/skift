
#include <libsystem/CString.h>
#include <libsystem/Logger.h>

#include "kernel/bus/PCI.h"
#include "kernel/x86/x86.h"

IterationDecision pci_device_iterate_bus(void *target, DeviceIterateCallback callback, int bus);

IterationDecision pci_device_iterate_hit(
    void *target,
    DeviceIterateCallback callback,
    PCIDevice device)
{
    device.vendor = (int)pci_device_read(device, PCI_VENDOR_ID, 2);
    device.device = (int)pci_device_read(device, PCI_DEVICE_ID, 2);

    return callback(target, (DeviceInfo){.bus = BUS_PCI, .pci_device = device});
}

IterationDecision pci_device_iterate_func(
    void *target,
    DeviceIterateCallback callback,
    PCIDevice device)
{
    if (pci_device_iterate_hit(target, callback, device) == ITERATION_STOP)
    {
        return ITERATION_STOP;
    }

    if (pci_device_type(device) == PCI_TYPE_BRIDGE)
    {
        return pci_device_iterate_bus(
            target,
            callback,
            pci_device_read(device, PCI_SECONDARY_BUS, 1));
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
    PCIDevice device = {
        .bus = bus,
        .slot = slot,
    };

    if (pci_device_read(device, PCI_VENDOR_ID, 2) == PCI_NONE)
        return ITERATION_CONTINUE;

    if (pci_device_iterate_func(target, callback, device) == ITERATION_STOP)
        return ITERATION_STOP;

    if (!pci_device_read(device, PCI_HEADER_TYPE, 1))
        return ITERATION_CONTINUE;

    for (int func = 1; func < 8; func++)
    {
        PCIDevice device = {
            .bus = bus,
            .slot = slot,
            .func = func,
        };

        if (pci_device_read(device, PCI_VENDOR_ID, 2) != PCI_NONE)
            if (pci_device_iterate_func(target, callback, device) == ITERATION_STOP)
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
    PCIDevice device = {};

    if ((pci_device_read(device, PCI_HEADER_TYPE, 1) & 0x80) == 0)
    {
        return pci_device_iterate_bus(target, callback, 0);
    }

    for (int func = 0; func < 8; ++func)
    {
        PCIDevice device = {.bus = 0, .slot = 0, .func = func};

        if (pci_device_read(device, PCI_VENDOR_ID, 2) == PCI_NONE)
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

static IterationDecision find_isa_bridge(PCIDevice *pci_isa, DeviceInfo info)
{
    if (info.pci_device.vendor != 0x8086)
        return ITERATION_CONTINUE;

    if (info.pci_device.device != 0x7000 ||
        info.pci_device.device != 0x7110)
        return ITERATION_CONTINUE;

    *pci_isa = info.pci_device;

    return ITERATION_STOP;
}

static bool has_pci_isa = false;
static PCIDevice pci_isa = {};
static uint8_t pci_remaps[4] = {};

void pci_remap(void)
{
    if (pci_device_iterate(&pci_isa, (DeviceIterateCallback)find_isa_bridge) == ITERATION_STOP)
    {
        has_pci_isa = true;

        for (int i = 0; i < 4; ++i)
        {
            pci_remaps[i] = pci_device_read(pci_isa, 0x60 + i, 1);

            if (pci_remaps[i] == 0x80)
            {
                pci_remaps[i] = 10 + (i % 1);
            }
        }

        uint32_t out = 0;
        memcpy(&out, &pci_remaps, 4);
        pci_device_write(pci_isa, 0x60, 4, out);
    }
}

int pci_get_interrupt(PCIDevice device)
{
    if (has_pci_isa)
    {
        uint32_t irq_pin = pci_device_read(device, 0x3D, 1);
        if (irq_pin == 0)
        {
            logger_error("PCI device does not specific interrupt line");
            return pci_device_read(device, PCI_INTERRUPT_LINE, 1);
        }

        int pirq = (irq_pin + device.slot - 2) % 4;
        int int_line = pci_device_read(device, PCI_INTERRUPT_LINE, 1);

        logger_info("slot is %d, irq pin is %d, so pirq is %d and that maps to %d? int_line=%d", device.slot, irq_pin, pirq, pci_remaps[pirq], int_line);

        for (int i = 0; i < 4; ++i)
        {
            logger_info("  irq[%d] = %d", i, pci_remaps[i]);
        }

        if (pci_remaps[pirq] >= 0x80)
        {
            logger_error("not mapped, remapping?");

            if (int_line == 0xFF)
            {
                int_line = 10;
                pci_device_write(device, PCI_INTERRUPT_LINE, 1, int_line);

                logger_warn("Just going in blind here.");
            }

            pci_remaps[pirq] = int_line;
            uint32_t out = 0;
            memcpy(&out, &pci_remaps, 4);
            pci_device_write(pci_isa, 0x60, 4, out);

            return int_line;
        }

        pci_device_write(device, PCI_INTERRUPT_LINE, 1, pci_remaps[pirq]);

        return pci_remaps[pirq];
    }
    else
    {
        return pci_device_read(device, PCI_INTERRUPT_LINE, 1);
    }
}
