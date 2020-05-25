#pragma once

#include <libsystem/Common.h>

typedef struct
{
    char name[100];
    u32 size;
    char *data;
} tar_block_t;

bool tar_read(void *tarfile, tar_block_t *block, uint index);
