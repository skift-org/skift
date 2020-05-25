#pragma once

#include "kernel/node/Node.h"

typedef struct
{
    size_t count;
    DirectoryEntry entries[];
} DirectoryListing;

typedef struct
{
    char name[FILE_NAME_LENGTH];
    FsNode *node;
} FsDirectoryEntry;

typedef struct
{
    FsNode node;
    List *childs;
} FsDirectory;

FsNode *directory_create(void);
