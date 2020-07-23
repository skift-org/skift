#include "kernel/virtio/Console.h"

bool virtio_console_match(DeviceInfo info)
{
    return virtio_is_specific_virtio_device(info, VIRTIO_DEVICE_CONSOLE);
}

void virtio_console_initialize(DeviceInfo info)
{
    __unused(info);
}
