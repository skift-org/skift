#pragma once

#include <libsystem/utils/List.h>

#include "kernel/node/Node.h"

struct FsSocket : public FsNode
{
    List *pending;
};

FsNode *socket_create();
