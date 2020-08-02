#pragma once

#include "kernel/node/Node.h"

struct FsDeviceInfo
{
    FsNode node;
};

void device_info_initialize();
