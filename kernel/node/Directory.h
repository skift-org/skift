#pragma once

#include "kernel/node/Node.h"

struct DirectoryListing
{
    size_t count;
    DirectoryEntry entries[];
};

struct FsDirectoryEntry
{
    char name[FILE_NAME_LENGTH];
    FsNode *node;
};

class FsDirectory : public FsNode
{
private:
    List *_childs;

public:
    FsDirectory();

    ~FsDirectory() override;

    Result open(FsHandle *handle) override;

    void close(FsHandle *handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;

    FsNode *find(const char *name) override;

    Result link(const char *name, FsNode *child) override;

    Result unlink(const char *name) override;
};
