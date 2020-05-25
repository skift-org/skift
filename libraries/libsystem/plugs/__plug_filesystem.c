
#include <abi/Syscalls.h>
#include <libsystem/Filesystem.h>
#include <libsystem/__plugs__.h>

Result filesystem_link(const char *oldpath, const char *newpath)
{
    return (Result)__syscall(SYS_FILESYSTEM_LINK, (int)oldpath, (int)newpath, 0, 0, 0);
}

Result filesystem_unlink(const char *path)
{
    return (Result)__syscall(SYS_FILESYSTEM_UNLINK, (int)path, 0, 0, 0, 0);
}

Result filesystem_mkdir(const char *path)
{
    return (Result)__syscall(SYS_FILESYSTEM_MKDIR, (int)path, 0, 0, 0, 0);
}

Result filesystem_mkpipe(const char *path)
{
    return (Result)__syscall(SYS_FILESYSTEM_MKPIPE, (int)path, 0, 0, 0, 0);
}

Result filesystem_rename(const char *old_path, const char *new_path)
{
    return (Result)__syscall(SYS_FILESYSTEM_RENAME, (int)old_path, (int)new_path, 0, 0, 0);
}
