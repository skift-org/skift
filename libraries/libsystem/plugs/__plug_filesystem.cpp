#include <abi/Syscalls.h>

#include <libsystem/core/CString.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Filesystem.h>

Result filesystem_link(const char *old_path, const char *new_path)
{
    return hj_filesystem_link(
        old_path, strnlen(old_path, PATH_LENGTH),
        new_path, strnlen(new_path, PATH_LENGTH));
}

Result filesystem_unlink(const char *path)
{
    return hj_filesystem_unlink(path, strnlen(path, PATH_LENGTH));
}

Result filesystem_mkdir(const char *path)
{
    return hj_filesystem_mkdir(path, strnlen(path, PATH_LENGTH));
}

Result filesystem_mkpipe(const char *path)
{
    return hj_filesystem_mkpipe(path, strnlen(path, PATH_LENGTH));
}

Result filesystem_rename(const char *old_path, const char *new_path)
{
    return hj_filesystem_rename(
        old_path, strnlen(old_path, PATH_LENGTH),
        new_path, strnlen(new_path, PATH_LENGTH));
}
