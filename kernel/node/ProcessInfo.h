#pragma once

#include "kernel/node/Node.h"

struct FsProcessInfo : public FsNode
{
    FsProcessInfo();
};

void process_info_initialize();
