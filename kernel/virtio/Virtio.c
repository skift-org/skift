#include <libsystem/system/Logger.h>

#include "kernel/bus/PCI.h"
#include "kernel/virtio/Virtio.h"

void virtio_device_initialize(DeviceInfo info)
{
    logger_info("Initializing virtIO device %s", device_to_static_string(info));
}

bool virtio_is_virtio_device(DeviceInfo info)
{
    return info.bus == BUS_PCI &&
           info.pci_device.vendor == 0x1AF4 &&
           info.pci_device.device >= 0x1000 &&
           info.pci_device.device < 0x1040;
}

bool virtio_is_specific_virtio_device(DeviceInfo info, int specific)
{
    if (!virtio_is_virtio_device(info))
    {
        return false;
    }

    return (int)pci_device_read(info.pci_device, PCI_SUBSYSTEM_ID, 2) == specific;
}
