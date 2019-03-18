#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/types.h>
#include <skift/utils.h>

typedef struct
{
    char name[100];
    u32 size;
    char *data;
} tar_block_t;

bool tar_read(void *tarfile, tar_block_t *block, uint index);