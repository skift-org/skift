#pragma once

#include <libsystem/list.h>

#include "node/Node.h"

typedef struct
{
    FsNode node;
    List *pending;
} FsSocket;

FsNode *socket_create(void);