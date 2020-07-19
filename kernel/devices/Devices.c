#include <libsystem/CString.h>
#include <libsystem/Logger.h>

#include "kernel/bus/PCI.h"
#include "kernel/devices/Devices.h"
#include "kernel/virtio/Network.h"
#include "kernel/virtio/Virtio.h"

static DeviceDriverInfo drivers[] = {
    {
        "BOCHS/QEMU Graphics Adaptor",
        BUS_PCI,
        bga_match,
        bga_initialize,
    },
    {
        "VirtIO Network Adaptor",
        BUS_PCI,
        virtio_network_match,
        virtio_network_initialize,
    },
    {NULL, BUS_NONE, NULL, NULL},
};

const DeviceDriverInfo *device_get_diver_info(DeviceInfo info)
{
    for (size_t i = 0; drivers[i].match; i++)
    {
        const DeviceDriverInfo *driver = &drivers[i];

        if (driver->bus == info.bus &&
            driver->match(info))
        {
            return driver;
        }
    }

    return NULL;
}

const char *device_to_static_string(DeviceInfo info)
{
    static char buffer[512];

    switch (info.bus)
    {
    case BUS_UNIX:
        snprintf(buffer, 512, "UNIX: %d", info.unix_device.device);
        break;

    default:
        snprintf(buffer, 512, "PCI: %02d:%02d.%d",
                 info.pci_device.bus,
                 info.pci_device.slot,
                 info.pci_device.func);
        break;
    }

    return buffer;
}

static IterationDecision print_device_info(void *target, DeviceInfo info)
{
    __unused(target);

    const DeviceDriverInfo *driver = device_get_diver_info(info);

    if (driver)
    {

        logger_info("%s: %s", device_to_static_string(info), driver->description);

        if (driver->initialize)
        {
            driver->initialize(info);
        }
    }
    else if (virtio_is_virtio_device(info))
    {
        logger_warn("%s: Unknown virtIO device", device_to_static_string(info));
    }
    else
    {
        logger_warn("%s: Unknown device", device_to_static_string(info));
    }

    return ITERATION_CONTINUE;
}

void device_initialize(void)
{
    logger_info("Initializing devices...");
    device_iterate(NULL, print_device_info);
}

void device_iterate(void *target, DeviceIterateCallback callback)
{
    if (pci_device_iterate(target, callback) == ITERATION_STOP)
    {
        return;
    }
}
