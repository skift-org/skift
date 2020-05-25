#pragma once

#include <libsystem/utils/List.h>

#include "kernel/node/Node.h"

typedef struct
{
    FsNode node;
    List *pending;
} FsSocket;

FsNode *socket_create(void);
