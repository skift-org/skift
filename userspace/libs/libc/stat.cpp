#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <abi/Syscalls.h>

int file_type_to_stat(HjFileType type)
{
    switch (type)
    {
    case HJ_FILE_TYPE_REGULAR:
        return _IFREG;
    case HJ_FILE_TYPE_DIRECTORY:
        return _IFDIR;
    default:
        // Log this
        return 0;
    }
}

void file_state_to_stat(HjStat *in, struct stat *out)
{
    out->st_size = in->size;
    out->st_mode = file_type_to_stat(in->type);
}

int stat(const char *path, struct stat *buf)
{
    int handle = open(path, HJ_OPEN_READ);

    if (handle == -1)
        return -1;

    int result = fstat(handle, buf);

    if (result == -1)
        return -1;

    close(handle);

    return 0;
}

int fstat(int fd, struct stat *buf)
{
    HjStat state;
    HjResult result = hj_handle_stat(fd, &state);
    file_state_to_stat(&state, buf);
    return result == HjResult::SUCCESS ? -1 : 0;
}

int mkdir(const char *pathname, mode_t mode)
{
    UNUSED(mode);

    return hj_filesystem_mkdir(pathname, strlen(pathname)) == HjResult::SUCCESS ? 0 : -1;
}
