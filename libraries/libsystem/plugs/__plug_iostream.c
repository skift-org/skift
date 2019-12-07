/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/iostream.h>
#include <libkernel/syscalls.h>

int __plug_iostream_open(const char *path, IOStreamFlag flags)
{
    return __syscall(SYS_FILESYSTEM_OPEN, (int)path, flags, 0, 0, 0);
}

int __plug_iostream_close(int fd)
{
    return __syscall(SYS_FILESYSTEM_CLOSE, fd, 0, 0, 0, 0);
}

int __plug_iostream_read(int fd, void *buffer, uint size)
{
    return __syscall(SYS_FILESYSTEM_READ, fd, (int)buffer, size, 0, 0);
}

int __plug_iostream_write(int fd, const void *buffer, uint size)
{
    return __syscall(SYS_FILESYSTEM_WRITE, fd, (int)buffer, size, 0, 0);
}

int __plug_iostream_call(int fd, int request, void *args)
{
    return __syscall(SYS_FILESYSTEM_CALL, fd, request, (int)args, 0, 0);
}

int __plug_iostream_seek(int fd, int offset, IOStreamWhence whence)
{
    return __syscall(SYS_FILESYSTEM_SEEK, fd, offset, whence, 0, 0);
}

int __plug_iostream_tell(int fd, IOStreamWhence whence)
{
    return __syscall(SYS_FILESYSTEM_TELL, fd, whence, 0, 0, 0);
}

int __plug_iostream_stat(int fd, IOStreamState *stat)
{
    return __syscall(SYS_FILESYSTEM_STAT, fd, (int)stat, 0, 0, 0);
}