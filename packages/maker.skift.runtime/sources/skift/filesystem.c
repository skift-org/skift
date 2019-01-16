#include <skift/syscalls.h>
#include <skift/filesystem.h>

int  sk_filesystem_open(const char *path, fsoflags_t flags)
{
    return __syscall(SYS_FILESYSTEM_OPEN, (int)path, (int)flags, 0, 0, 0);
}

void sk_filesystem_close(int fd)
{
    __syscall(SYS_FILESYSTEM_CLOSE, fd, 0, 0, 0, 0);
}

int sk_filesystem_read (int fd, void *buffer, uint size)
{
    return __syscall(SYS_FILESYSTEM_READ, fd, (int)buffer, (int)size, 0, 0);
}

int sk_filesystem_write(int fd, void *buffer, uint size)
{
    return __syscall(SYS_FILESYSTEM_WRITE, fd, (int)buffer, (int)size, 0, 0);
}

int sk_filesystem_seek(int fd, int offset, seek_origin_t origin)
{
    return __syscall(SYS_FILESYSTEM_SEEK, fd, offset, origin, 0, 0);
}

int sk_filesystem_tell(int fd)
{
    return __syscall(SYS_FILESYSTEM_TELL, fd, 0, 0, 0, 0);
}

int sk_filesystem_fstat(int fd, file_stat_t *stat)
{
    return __syscall(SYS_FILESYSTEM_FSTAT, fd, (int)stat, 0, 0, 0);
}

int filesystem_mkdir(const char *path)
{
    return __syscall(SYS_FILESYSTEM_MKDIR, (int)path, 0, 0, 0, 0);
}

int filesystem_rm(const char *path)
{
    return __syscall(SYS_FILESYSTEM_RM, (int)path, 0, 0, 0, 0);
}