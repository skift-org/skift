#pragma once

#include "kernel/virtio/Virtio.h"

bool virtio_console_match(DeviceInfo info);

void virtio_console_initialize(DeviceInfo info);
