#pragma once

#include "kernel/node/Node.h"

struct FsDeviceInfo : public FsNode
{
    FsDeviceInfo();
};

void device_info_initialize();
