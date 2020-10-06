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
public:
    List *childs;

    FsDirectory();

    ~FsDirectory() override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;
};
