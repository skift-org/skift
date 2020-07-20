#pragma once

#include "kernel/virtio/Virtio.h"

bool virtio_block_match(DeviceInfo info);

void virtio_block_initialize(DeviceInfo info);
