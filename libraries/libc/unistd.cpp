#include <string.h>
#include <unistd.h>

#include <abi/Syscalls.h>

ssize_t read(int fd, void *buf, size_t count)
{
    size_t read = 0;
    Result result = hj_handle_read(fd, buf, count, &read);

    if (result != Result::SUCCESS)
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
    size_t written = 0;
    Result result = hj_handle_write(fd, buf, count, &written);

    if (result != Result::SUCCESS)
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
    return hj_filesystem_unlink(pathname, strlen(pathname)) == Result::SUCCESS ? 0 : -1;
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