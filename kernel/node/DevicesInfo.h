#pragma once

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;
} FsDeviceInfo;

FsNode *device_info_create(void);

void device_info_initialize(void);
