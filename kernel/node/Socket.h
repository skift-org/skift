#pragma once

#include <libsystem/utils/List.h>

#include "kernel/node/Node.h"

struct FsSocket : public FsNode
{
private:
    List *_pending;

public:
    FsSocket();

    ~FsSocket() override;

    ResultOr<FsNode *> connect() override;

    bool can_accept() override;

    ResultOr<FsNode *> accept() override;
};
