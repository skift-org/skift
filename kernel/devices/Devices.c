#include <libsystem/logger.h>

#include "kernel/bus/PCI.h"
#include "kernel/devices/Devices.h"
#include "kernel/virtio/Virtio.h"

static DeviceDriverInfo drivers[] = {
    {"BOCHS/QEMU Graphics Adaptor", BUS_PCI, bga_match, bga_initialize},
    {"VirtIO Network Adaptor", BUS_PCI, virtio_net_match, virtio_net_initialize},
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

static IterationDecision print_device_info(void *target, DeviceInfo info)
{
    __unused(target);

    const DeviceDriverInfo *driver = device_get_diver_info(info);

    if (driver)
    {
        logger_info("Found '%s' PCI:%06x:%4x:%4x",
                    driver->description,
                    info.device,
                    info.vendor_id,
                    info.device_id);

        if (driver->initialize)
        {
            driver->initialize(info);
        }
    }
    else
    {
        logger_warn("Unknown device: PCI:%06x:%4x:%4x", info.device, info.vendor_id, info.device_id);
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
