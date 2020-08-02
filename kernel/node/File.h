#pragma once

#include "kernel/node/Node.h"

struct FsFile : public FsNode
{
    char *buffer;
    size_t buffer_allocated;
    size_t buffer_size;
};

FsNode *file_create();
