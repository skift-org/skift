#include <fcntl.h>
#include <string.h>

#include <abi/Syscalls.h>

OpenFlag fcntl_parse_mode(int mode)
{
    OpenFlag flags = OPEN_STREAM;

    // Open Mode
    if (mode == O_RDONLY)
    {
        flags |= OPEN_READ;
    }
    else if (mode == O_WRONLY)
    {
        flags |= OPEN_WRITE;
    }
    else if (mode == O_RDWR)
    {
        flags |= OPEN_READ | OPEN_WRITE;
    }

    // Flags
    if (mode & O_CREAT)
    {
        flags |= OPEN_CREATE;
    }
    if (mode & O_APPEND)
    {
        flags |= OPEN_APPEND;
    }
    if (mode & O_DIRECTORY)
    {
        flags |= OPEN_DIRECTORY;
    }

    return flags;
}

int open(const char *path, int mode, ...)
{
    int flags = fcntl_parse_mode(mode);
    int handle = 0;
    Result result = hj_handle_open(&handle, path, strlen(path), flags);

    if (result != Result::SUCCESS)
    {
        // TODO: set errno
        return -1;
    }

    return handle;
}