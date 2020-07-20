#pragma once

#include "kernel/virtio/Virtio.h"

bool virtio_graphic_match(DeviceInfo info);

void virtio_graphic_initialize(DeviceInfo info);
