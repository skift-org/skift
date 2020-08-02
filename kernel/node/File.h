#pragma once

#include "kernel/node/Node.h"

struct FsFile
{
    FsNode node;

    char *buffer;
    size_t realsize;
    size_t size;
};

FsNode *file_create();
