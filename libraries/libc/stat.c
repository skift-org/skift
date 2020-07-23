#include <libsystem/io/Filesystem.h>
#include <libsystem/system/Logger.h>
#include <sys/stat.h>

int mkdir(const char *pathname, mode_t mode)
{
    __unused(mode);
    logger_trace("mkdir(%s, %04o)", pathname, mode);

    return filesystem_mkdir(pathname);
}
