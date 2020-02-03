/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/filesystem.h>
#include <abi/Syscalls.h>

int filesystem_link(const char *oldpath, const char *newpath)
{
    return __syscall(SYS_FILESYSTEM_LINK, (int)oldpath, (int)newpath, 0, 0, 0);
}

int filesystem_unlink(const char *path)
{
    return __syscall(SYS_FILESYSTEM_UNLINK, (int)path, 0, 0, 0, 0);
}

int filesystem_mkdir(const char *path)
{
    return __syscall(SYS_FILESYSTEM_MKDIR, (int)path, 0, 0, 0, 0);
}

int filesystem_mkpipe(const char *path)
{
    return __syscall(SYS_FILESYSTEM_MKPIPE, (int)path, 0, 0, 0, 0);
}

int filesystem_rename(const char *old_path, const char *new_path)
{
    return __syscall(SYS_FILESYSTEM_RENAME, (int)old_path, (int)new_path, 0, 0, 0);
}