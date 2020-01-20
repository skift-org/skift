#pragma once

#include "node/Node.h"

typedef struct
{
    FsNode node;
} FsSocket;

FsNode *socket_create(void);