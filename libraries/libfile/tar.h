#pragma once

#include <libsystem/Common.h>

typedef struct
{
    char name[100];
    u32 size;
    char *data;
} TARBlock;

bool tar_read(void *tarfile, TARBlock *block, uint index);
