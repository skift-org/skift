#pragma once

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;
} FsProcessInfo;

FsNode *process_info_create(void);

void process_info_initialize(void);
