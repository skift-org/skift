#pragma once

#include <libutils/Vector.h>

#include "system/node/Node.h"

struct FileListing
{
    size_t count;
    HjDirEntry entries[];
};

struct FsHjDirEntry
{
    String name;
    RefPtr<FsNode> node;
};

struct FsDirectory : public FsNode
{
private:
    Vector<FsHjDirEntry> _children{};

public:
    FsDirectory();

    HjResult open(FsHandle &handle) override;

    void close(FsHandle &handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;

    RefPtr<FsNode> find(String name) override;

    HjResult link(String name, RefPtr<FsNode> child) override;

    HjResult unlink(String name) override;
};
