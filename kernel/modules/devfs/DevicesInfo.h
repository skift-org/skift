#pragma once

#include "system/node/Node.h"

struct FsDeviceInfo : public FsNode
{
private:
public:
    FsDeviceInfo();

    HjResult open(FsHandle &handle) override;

    void close(FsHandle &handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;
};

void device_info_initialize();
