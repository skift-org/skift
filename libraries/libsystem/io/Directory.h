#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/io/Handle.h>

typedef struct
{
    Handle handle;
} Directory;

Directory *directory_open(const char *path, OpenFlag flags);

void directory_close(Directory *directory);

int directory_read(Directory *directory, DirectoryEntry *entry);