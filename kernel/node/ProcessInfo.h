#pragma once

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;
} FsProcessInfo;

void process_info_initialize(void);
