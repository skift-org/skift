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
    char name[FILE_NAME_LENGTH];
    RefPtr<FsNode> node;
};

class FsDirectory : public FsNode
{
private:
    Vector<FsDirectoryEntry> _childs{};

public:
    FsDirectory();

    Result open(FsHandle *handle) override;

    void close(FsHandle *handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;

    RefPtr<FsNode> find(const char *name) override;

    Result link(const char *name, RefPtr<FsNode> child) override;

    Result unlink(const char *name) override;
};
