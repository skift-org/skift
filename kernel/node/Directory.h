#pragma once

#include <libutils/Vector.h>

#include "kernel/node/Node.h"

struct DirectoryListing
{
    size_t count;
    DirectoryEntry entries[];
};

struct FsDirectoryEntry
{
    String name;
    RefPtr<FsNode> node;
};

class FsDirectory : public FsNode
{
private:
    Vector<FsDirectoryEntry> _childs{};

public:
    FsDirectory();

    Result open(FsHandle &handle) override;

    void close(FsHandle &handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;

    RefPtr<FsNode> find(String name) override;

    Result link(String name, RefPtr<FsNode> child) override;

    Result unlink(String name) override;
};
