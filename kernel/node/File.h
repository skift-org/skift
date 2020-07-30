#pragma once

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;

    char *buffer;
    size_t realsize;
    size_t size;
} FsFile;

FsNode *file_create();
