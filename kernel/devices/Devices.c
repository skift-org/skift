#include <libsystem/logger.h>

#include "kernel/bus/PCI.h"

static DeviceDriver drivers[] = {
    {"BOCHS/QEMU Graphics Adaptor", BUS_PCI, 0x1234, 0x1111, bga_initialize},
    {NULL, BUS_NONE, 0, 0, NULL},
};

IterationDecision
print_device_info(void *target, DeviceInfo info)
{
    __unused(target);

    for (size_t i = 0; drivers[i].initialize; i++)
    {
        DeviceDriver *driver = &drivers[i];

        if (driver->bus == info.bus &&
            driver->vendor_id == info.vendor_id &&
            driver->device_id == info.device_id)
        {
            logger_info("Found '%s' PCI:%06x:%4x:%4x", drivers[i].description, info.device, info.vendor_id, info.device_id);

            if (driver->initialize)
            {
                driver->initialize(info);
            }

            return ITERATION_CONTINUE;
        }
    }

    logger_warn("Unknown device: PCI:%06x:%4x:%4x", info.device, info.vendor_id, info.device_id);

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
