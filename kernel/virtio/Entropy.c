#include "kernel/virtio/Entropy.h"

bool virtio_entropy_match(DeviceInfo info)
{
    return virtio_is_specific_virtio_device(info, VIRTIO_DEVICE_ENTROPY);
}

void virtio_entropy_initialize(DeviceInfo info)
{
    __unused(info);
}
