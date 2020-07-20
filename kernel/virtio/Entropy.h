#pragma once

#include "kernel/virtio/Virtio.h"

bool virtio_entropy_match(DeviceInfo info);

void virtio_entropy_initialize(DeviceInfo info);
