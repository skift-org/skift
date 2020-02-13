/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/io/Directory.h>

Directory *directory_open(const char *path, OpenFlag flags)
{
    Directory *directory = __create(Directory);

    __plug_handle_open(HANDLE(directory), path, flags | OPEN_DIRECTORY);

    return directory;
}

void directory_close(Directory *directory)
{
    __plug_handle_close(HANDLE(directory));

    free(directory);
}

int directory_read(Directory *directory, DirectoryEntry *entry)
{
    return __plug_handle_read(HANDLE(directory), entry, sizeof(DirectoryEntry));
}
