#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/RingBuffer.h>
#include <libsystem/lock.h>

#include "kernel/node/Node.h"

struct FsTerminalEndpoint;

typedef struct
{
    FsNode node;

    int width;
    int height;

    RingBuffer *master_to_slave_buffer;
    RingBuffer *slave_to_master_buffer;
} FsTerminal;

FsNode *terminal_create(void);