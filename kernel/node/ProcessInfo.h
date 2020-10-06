#pragma once

#include "kernel/node/Node.h"

class FsProcessInfo : public FsNode
{
private:
public:
    FsProcessInfo();

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;
};

void process_info_initialize();
