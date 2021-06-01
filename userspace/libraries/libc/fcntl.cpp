#include <fcntl.h>
#include <string.h>

#include <abi/Syscalls.h>

HjOpenFlag fcntl_parse_mode(int mode)
{
    HjOpenFlag flags = HJ_OPEN_STREAM;

    // Open Mode
    if (mode == O_RDONLY)
    {
        flags |= HJ_OPEN_READ;
    }
    else if (mode == O_WRONLY)
    {
        flags |= HJ_OPEN_WRITE;
    }
    else if (mode == O_RDWR)
    {
        flags |= HJ_OPEN_READ | HJ_OPEN_WRITE;
    }

    // Flags
    if (mode & O_CREAT)
    {
        flags |= HJ_OPEN_CREATE;
    }
    if (mode & O_APPEND)
    {
        flags |= HJ_OPEN_APPEND;
    }
    if (mode & O_DIRECTORY)
    {
        flags |= HJ_OPEN_DIRECTORY;
    }

    return flags;
}

int open(const char *path, int mode, ...)
{
    int flags = fcntl_parse_mode(mode);
    int handle = 0;
    HjResult result = hj_handle_open(&handle, path, strlen(path), flags);

    if (result != HjResult::SUCCESS)
    {
        // TODO: set errno
        return -1;
    }

    return handle;
}