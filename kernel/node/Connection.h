#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/utils/List.h>

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;

    bool accepted;
    bool is_open;

    List *message_to_server;
    List *message_to_client;
} FsConnection;

FsNode *connection_create(void);