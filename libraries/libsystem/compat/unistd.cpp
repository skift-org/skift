#include <unistd.h>

#include <abi/Syscalls.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Filesystem.h>

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

off_t lseek(int fd, off_t offset, int whence)
{
    Result result = hj_handle_seek(fd, offset, (Whence)whence);
    if (result != Result::SUCCESS)
    {
        // TODO: set errno
        return (off_t)-1;
    }

    int offset_beg;
    hj_handle_tell(fd, WHENCE_START, &offset_beg);
    return offset_beg;
}