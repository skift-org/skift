#pragma once

#include <libutils/Vec.h>

#include "system/node/Node.h"

struct FsSocket : public FsNode
{
private:
    Vec<RefPtr<FsNode>> _pending{};

public:
    FsSocket();

    ResultOr<RefPtr<FsNode>> connect() override;

    bool can_accept() override;

    ResultOr<RefPtr<FsNode>> accept() override;
};
