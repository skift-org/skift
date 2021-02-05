#include <unistd.h>

#include <libsystem/io/Filesystem.h>
#include <libsystem/core/Plugs.h>
#include <abi/Syscalls.h>

ssize_t read(int fd, void *buf, size_t count)
{
    Handle hnd = {
        .id = fd,
        .flags = 0,
        .result = SUCCESS,
    };

    size_t read = __plug_handle_read(&hnd, buf, count);

    if (handle_has_error(&hnd))
    {
        return -1;
    }
    else
    {
        return (ssize_t)read;
    }
}

ssize_t write(int fd, const void *buf, size_t count)
{
    Handle hnd = {
        .id = fd,
        .flags = 0,
        .result = SUCCESS,
    };

    size_t written = __plug_handle_write(&hnd, buf, count);

    if (handle_has_error(&hnd))
    {
        return -1;
    }
    else
    {
        return (ssize_t)written;
    }
}

int unlink(const char *pathname)
{
    return filesystem_unlink(pathname) == Result::SUCCESS ? 0 : -1;
}

int rmdir(const char *dirname)
{
    return unlink(dirname);
}

int close(int fd)
{
    return hj_handle_close(fd) == Result::SUCCESS ? 0 : -1;
}