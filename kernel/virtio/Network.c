#include <libsystem/system/Logger.h>

#include "kernel/bus/PCI.h"
#include "kernel/virtio/Virtio.h"

bool virtio_network_match(DeviceInfo info)
{
    return virtio_is_specific_virtio_device(info, VIRTIO_DEVICE_NETWORK);
}

void virtio_network_initialize(DeviceInfo info)
{
    virtio_device_initialize(info);
}
