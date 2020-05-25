#include <libsystem/logger.h>

#include "kernel/bus/PCI.h"
#include "kernel/virtio/Virtio.h"

bool virtio_net_match(DeviceInfo info)
{
    return info.pci_device.vendor == 0x1AF4 &&
           info.pci_device.device >= 0x1000 &&
           info.pci_device.device < 0x1040 &&
           pci_device_read(info.pci_device, PCI_SUBSYSTEM_ID, 2) == 0x01;
}

void virtio_net_initialize(DeviceInfo info)
{
    __unused(info);
}
