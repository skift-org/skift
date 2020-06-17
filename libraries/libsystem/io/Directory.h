#pragma once

#include <libsystem/io/Handle.h>

typedef struct Directory Directory;

Directory *directory_open(const char *path, OpenFlag flags);

void directory_close(Directory *directory);

int directory_read(Directory *directory, DirectoryEntry *entry);
