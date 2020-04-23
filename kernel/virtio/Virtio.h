#pragma once

#include "kernel/devices/Devices.h"

bool virtio_net_match(DeviceInfo info);

void virtio_net_initialize(DeviceInfo info);