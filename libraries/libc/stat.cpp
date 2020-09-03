#include <libsystem/Logger.h>
#include <libsystem/io/Filesystem.h>
#include <sys/stat.h>

int mkdir(const char *pathname, mode_t mode)
{
    __unused(mode);

    return filesystem_mkdir(pathname);
}
