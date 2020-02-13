#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;

    byte *buffer;
    size_t realsize;
    size_t size;
} FsFile;

FsNode *file_create(void);