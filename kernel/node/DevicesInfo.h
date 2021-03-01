#pragma once

#include "kernel/node/Node.h"

class FsDeviceInfo : public FsNode
{
private:
public:
    FsDeviceInfo();

    Result open(FsHandle &handle) override;

    void close(FsHandle &handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;
};

void device_info_initialize();
