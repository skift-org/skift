#pragma once

#include <assert.h>
#include <libutils/Iteration.h>
#include <stdio.h>

#include "pci/PCIAddress.h"
#include "ps2/LegacyAddress.h"
#include "unix/UNIXAddress.h"
#include "virtio/VirtioAddress.h"

enum DeviceBus
{
    BUS_NONE,

    BUS_UNIX,
    BUS_PCI,
    BUS_LEGACY
};

class DeviceAddress
{
private:
    DeviceBus _bus;

    union
    {
        LegacyAddress _legacy;
        PCIAddress _pci;
        UNIXAddress _unix;
    };

public:
    DeviceBus bus()
    {
        return _bus;
    }

    LegacyAddress legacy()
    {
        assert(_bus == BUS_LEGACY);
        return _legacy;
    };

    PCIAddress pci()
    {
        assert(_bus == BUS_PCI);
        return _pci;
    };

    UNIXAddress unix()
    {
        assert(_bus == BUS_UNIX);
        return _unix;
    };

    DeviceAddress()
        : _bus(BUS_NONE)
    {
    }

    DeviceAddress(LegacyAddress address)
        : _bus(BUS_LEGACY),
          _legacy(address)
    {
    }

    DeviceAddress(PCIAddress address)
        : _bus(BUS_PCI),
          _pci(address)
    {
    }

    DeviceAddress(UNIXAddress address)
        : _bus(BUS_UNIX),
          _unix(address)
    {
    }

    const char *as_static_cstring()
    {
        static char buffer[128];

        switch (_bus)
        {
        case BUS_LEGACY:
            snprintf(buffer, 127, "legacy:%d", legacy());
            break;

        case BUS_UNIX:
            snprintf(buffer, 127, "unix:%d", unix());
            break;

        case BUS_PCI:
            snprintf(buffer, 127, "pci:%02x:%02x.%x %04x:%04x",
                     pci().bus(),
                     pci().slot(),
                     pci().func(),
                     pci().read16(PCI_VENDOR_ID),
                     pci().read16(PCI_DEVICE_ID));
            break;

        default:
            snprintf(buffer, 127, "none");
            break;
        }

        return buffer;
    }
};
