#pragma once

#include "kernel/virtio/Virtio.h"

bool virtio_network_match(DeviceInfo info);

void virtio_network_initialize(DeviceInfo info);
