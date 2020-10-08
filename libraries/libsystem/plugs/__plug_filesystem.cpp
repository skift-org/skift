
#include <abi/Syscalls.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Filesystem.h>

Result filesystem_link(const char *oldpath, const char *newpath)
{
    return __syscall(SYS_FILESYSTEM_LINK, (uintptr_t)oldpath, (uintptr_t)newpath);
}

Result filesystem_unlink(const char *path)
{
    return __syscall(SYS_FILESYSTEM_UNLINK, (uintptr_t)path);
}

Result filesystem_mkdir(const char *path)
{
    return __syscall(SYS_FILESYSTEM_MKDIR, (uintptr_t)path);
}

Result filesystem_mkpipe(const char *path)
{
    return __syscall(SYS_FILESYSTEM_MKPIPE, (uintptr_t)path);
}

Result filesystem_rename(const char *old_path, const char *new_path)
{
    return __syscall(SYS_FILESYSTEM_RENAME, (uintptr_t)old_path, (uintptr_t)new_path);
}
