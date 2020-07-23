#include <kernel/virtio/Block.h>

bool virtio_block_match(DeviceInfo info)
{
    return virtio_is_specific_virtio_device(info, VIRTIO_DEVICE_BLOCK);
}

void virtio_block_initialize(DeviceInfo info)
{
    __unused(info);
}
