#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;
} FsDeviceInfo;

FsNode *device_info_create(void);

void device_info_initialize(void);