#include "kernel/virtio/Graphic.h"

bool virtio_graphic_match(DeviceInfo info)
{
    return virtio_is_specific_virtio_device(info, VIRTIO_DEVICE_GRAPHICS);
}

void virtio_graphic_initialize(DeviceInfo info)
{
    __unused(info);
}
