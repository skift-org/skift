#include <stdio.h>
#include <sys/stat.h>

#include <libsystem/Logger.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>

int file_type_to_stat(FileType type)
{
    switch (type)
    {
    case FILE_TYPE_REGULAR:
        return _IFREG;
    case FILE_TYPE_DIRECTORY:
        return _IFDIR;
    default:
        logger_error("[STAT] Unhandled filetype: %u", type);
        return 0;
    }
}

void file_state_to_stat(FileState *in, struct stat *out)
{
    out->st_size = in->size;
    out->st_mode = file_type_to_stat(in->type);
}

int stat(const char *path, struct stat *buf)
{
    Stream* stream = stream_open(path, OPEN_READ);

    FileState stat;
    stream_stat((Stream *)stream, &stat);
    file_state_to_stat(&stat, buf);

    stream_close(stream);

    return 0;
}

int fstat(int fd, struct stat *buf)
{
    Stream* stream = stream_open_handle(fd, OPEN_READ);

    FileState stat;
    stream_stat(stream, &stat);
    file_state_to_stat(&stat, buf);

    stream_close(stream);

    return 0;
}

int mkdir(const char *pathname, mode_t mode)
{
    __unused(mode);

    return filesystem_mkdir(pathname);
}
