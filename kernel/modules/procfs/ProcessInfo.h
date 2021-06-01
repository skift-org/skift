#pragma once

#include "system/node/Node.h"

struct FsProcessInfo : public FsNode
{
private:
public:
    FsProcessInfo();

    HjResult open(FsHandle &handle) override;

    void close(FsHandle &handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;
};

void process_info_initialize();
