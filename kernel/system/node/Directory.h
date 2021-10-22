#pragma once

#include <libutils/Vec.h>

#include "system/node/Node.h"

struct FileListing
{
    size_t count;
    HjDirEntry entries[];
};

struct FsHjDirEntry
{
    String name;
    Ref<FsNode> node;
};

struct FsDirectory : public FsNode
{
private:
    Vec<FsHjDirEntry> _children{};

public:
    FsDirectory();

    HjResult open(FsHandle &handle) override;

    void close(FsHandle &handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;

    Ref<FsNode> find(String name) override;

    HjResult link(String name, Ref<FsNode> child) override;

    HjResult unlink(String name) override;
};
