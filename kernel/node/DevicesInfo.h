#pragma once

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;
} FsDeviceInfo;

void device_info_initialize();
