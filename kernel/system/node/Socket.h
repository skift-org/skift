#pragma once

#include <libutils/Vec.h>

#include "system/node/Node.h"

struct FsSocket : public FsNode
{
private:
    Vec<Ref<FsNode>> _pending{};

public:
    FsSocket();

    ResultOr<Ref<FsNode>> connect() override;

    bool can_accept() override;

    ResultOr<Ref<FsNode>> accept() override;
};
