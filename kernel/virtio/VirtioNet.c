#include <libsystem/logger.h>

#include "kernel/bus/PCI.h"
#include "kernel/virtio/Virtio.h"

bool virtio_net_match(DeviceInfo info)
{
    return info.vendor_id == 0x1AF4 &&
           info.device_id >= 0x1000 &&
           info.device_id < 0x1040 &&
           pci_read_field(info.device, PCI_SUBSYSTEM_ID, 2) == 0x01;
}

void virtio_net_initialize(DeviceInfo info)
{
    __unused(info);
}