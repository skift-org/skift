#pragma once

#include <libsystem/io/Handle.h>

struct Directory;

Directory *directory_open(const char *path, OpenFlag flags);

void directory_close(Directory *directory);

int directory_read(Directory *directory, DirectoryEntry *entry);

bool directory_exist(const char *path);
