
#include <abi/Syscalls.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Filesystem.h>

Result filesystem_link(const char *oldpath, const char *newpath)
{
    return __syscall(SYS_FILESYSTEM_LINK, (int)oldpath, (int)newpath);
}

Result filesystem_unlink(const char *path)
{
    return __syscall(SYS_FILESYSTEM_UNLINK, (int)path);
}

Result filesystem_mkdir(const char *path)
{
    return __syscall(SYS_FILESYSTEM_MKDIR, (int)path);
}

Result filesystem_mkpipe(const char *path)
{
    return __syscall(SYS_FILESYSTEM_MKPIPE, (int)path);
}

Result filesystem_rename(const char *old_path, const char *new_path)
{
    return __syscall(SYS_FILESYSTEM_RENAME, (int)old_path, (int)new_path);
}
