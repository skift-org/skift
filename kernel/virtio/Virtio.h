#pragma once

#include "kernel/devices/Devices.h"

// 2.1 Device Status Field

#define VIRTIO_STATUS_ACKNOWLEDGE (1)
#define VIRTIO_STATUS_DRIVER (2)
#define VIRTIO_STATUS_DRIVER_OK (4)
#define VIRTIO_STATUS_FAILED (128)
#define VIRTIO_STATUS_FEATURES_OK (8)
#define VIRTIO_STATUS_DEVICE_NEEDS_RESET (64)

#define VIRTIO_REGISTER_DEVICE_FEATURES (0x00)
#define VIRTIO_REGISTER_GUEST_FEATURES (0x04)
#define VIRTIO_REGISTER_QUEUE_ADDRESS (0x08)
#define VIRTIO_REGISTER_QUEUE_SIZE (0x0C)
#define VIRTIO_REGISTER_QUEUE_SELECT (0x0E)
#define VIRTIO_REGISTER_QUEUE_NOTIFY (0x10)
#define VIRTIO_REGISTER_DEVICE_STATUS (0x12)
#define VIRTIO_REGISTER_ISR_STATUS (0x13)

#define VIRTIO_DEVICE_NETWORK (1)
#define VIRTIO_DEVICE_BLOCK (2)
#define VIRTIO_DEVICE_CONSOLE (3)
#define VIRTIO_DEVICE_ENTROPY (4)
#define VIRTIO_DEVICE_GRAPHICS (16)

void virtio_device_initialize(DeviceInfo info);

bool virtio_is_virtio_device(DeviceInfo info);

bool virtio_is_specific_virtio_device(DeviceInfo info, int specific);
