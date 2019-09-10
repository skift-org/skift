#include <sys/stat.h>
#include <libsystem/filesystem.h>

int mkdir(const char *pathname, mode_t mode)
{
    UNUSED(mode);
    return filesystem_mkdir(pathname);
}