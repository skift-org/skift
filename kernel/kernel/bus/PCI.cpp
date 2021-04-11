
#include <libsystem/Logger.h>
#include <libutils/Vector.h>
#include <string.h>

#include "archs/x86_32/x86_32.h"
#include "kernel/bus/PCI.h"

Iteration pci_scan_bus(IterationCallback<PCIAddress> &callback, int bus);

Iteration pci_scan_hit(IterationCallback<PCIAddress> &callback, PCIAddress &address)
{
    return callback(address);
}

Iteration pci_scan_func(IterationCallback<PCIAddress> &callback, PCIAddress &address)
{
    if (pci_scan_hit(callback, address) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (address.read_class_sub_class() == PCI_TYPE_BRIDGE)
    {
        return pci_scan_bus(callback, address.read8(PCI_SECONDARY_BUS));
    }
    else
    {
        return Iteration::CONTINUE;
    }
}

Iteration pci_scan_slot(IterationCallback<PCIAddress> &callback, int bus, int slot)
{
    PCIAddress address{bus, slot, 0};

    if (address.read16(PCI_VENDOR_ID) == PCI_NONE)
    {
        return Iteration::CONTINUE;
    }

    if (pci_scan_func(callback, address) == Iteration::STOP)
    {
        return Iteration::STOP;
    }

    if (!address.read8(PCI_HEADER_TYPE))
    {
        return Iteration::CONTINUE;
    }

    for (int func = 1; func < 8; func++)
    {
        PCIAddress address{bus, slot, func};

        if (address.read16(PCI_VENDOR_ID) != PCI_NONE &&
            pci_scan_func(callback, address) == Iteration::STOP)
        {
            return Iteration::STOP;
        }
    }

    return Iteration::CONTINUE;
}

Iteration pci_scan_bus(IterationCallback<PCIAddress> &callback, int bus)
{
    for (int slot = 0; slot < 32; ++slot)
    {
        if (pci_scan_slot(callback, bus, slot) == Iteration::STOP)
        {
            return Iteration::STOP;
        }
    }

    return Iteration::CONTINUE;
}

Iteration pci_scan(IterationCallback<PCIAddress> callback)
{
    PCIAddress address{};

    if ((address.read8(PCI_HEADER_TYPE) & 0x80) == 0)
    {
        return pci_scan_bus(callback, 0);
    }

    for (int func = 0; func < 8; ++func)
    {
        PCIAddress address{0, 0, func};

        if (address.read16(PCI_VENDOR_ID) == PCI_NONE)
        {
            return Iteration::CONTINUE;
        }

        if (pci_scan_bus(callback, func) == Iteration::STOP)
        {
            return Iteration::STOP;
        }
    }

    return Iteration::CONTINUE;
}

static bool _has_isa_bridge = false;
static PCIAddress _isa_bridge_address = {};
static uint8_t _isa_remaps[4] = {};

void pci_initialize_isa_bridge()
{
    auto is_isa_bridge = [](PCIAddress &address) {
        return address.read16(PCI_CLASS) == 0x06 &&
               address.read16(PCI_SUBCLASS) == 0x01;
    };

    pci_scan([&](PCIAddress address) {
        if (is_isa_bridge(address))
        {
            logger_info("Found isa bridge on PCI:%02x:%02x.%x", address.bus(), address.slot(), address.func());
            _isa_bridge_address = address;
            _has_isa_bridge = true;

            for (int i = 0; i < 4; ++i)
            {
                _isa_remaps[i] = address.read8(0x60 + i);

                if (_isa_remaps[i] == 0x80)
                {
                    _isa_remaps[i] = 10 + (i % 1);
                }
            }

            uint32_t out = 0;
            memcpy(&out, &_isa_remaps, 4);
            address.write32(0x60, out);

            return Iteration::STOP;
        }
        else
        {
            return Iteration::CONTINUE;
        }
    });
}

int pci_get_interrupt(PCIAddress address)
{
    if (_has_isa_bridge)
    {
        uint32_t irq_pin = address.read8(PCI_INTERRUPT_PIN);
        if (irq_pin == 0)
        {
            logger_warn("PCI device does not specify an interrupt line!");
            return address.read8(PCI_INTERRUPT_LINE);
        }

        int pirq = (irq_pin + address.slot() - 2) % 4;
        int int_line = address.read8(PCI_INTERRUPT_LINE);

        logger_info("Slot is %d, irq pin is %d, so pirq is %d and that maps to %d? int_line=%d",
                    address.slot(), irq_pin, pirq, _isa_remaps[pirq], int_line);

        for (int i = 0; i < 4; ++i)
        {
            logger_info("  irq[%d] = %d", i, _isa_remaps[i]);
        }

        if (_isa_remaps[pirq] >= 0x80)
        {
            logger_info("Not mapped, remapping?");

            if (int_line == 0xFF)
            {
                logger_warn("Just going in blind here.");
                int_line = 10;
                address.write8(PCI_INTERRUPT_LINE, 1);
            }

            _isa_remaps[pirq] = int_line;
            uint32_t out = 0;
            memcpy(&out, &_isa_remaps, 4);
            _isa_bridge_address.write32(0x60, out);

            return int_line;
        }

        address.write8(PCI_INTERRUPT_LINE, _isa_remaps[pirq]);

        return _isa_remaps[pirq];
    }
    else
    {
        return address.read8(PCI_INTERRUPT_LINE);
    }
}

void pci_initialize()
{
    pci_initialize_isa_bridge();
}
