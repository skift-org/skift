#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/utils/List.h>

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;
    List *pending;
} FsSocket;

FsNode *socket_create(void);