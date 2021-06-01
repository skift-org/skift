#include <string.h>
#include <unistd.h>

#include <abi/Syscalls.h>

ssize_t read(int fd, void *buf, size_t count)
{
    size_t read = 0;
    HjResult result = hj_handle_read(fd, buf, count, &read);

    if (result != HjResult::SUCCESS)
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
    HjResult result = hj_handle_write(fd, buf, count, &written);

    if (result != HjResult::SUCCESS)
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
    return hj_filesystem_unlink(pathname, strlen(pathname)) == HjResult::SUCCESS ? 0 : -1;
}

int rmdir(const char *dirname)
{
    return unlink(dirname);
}

int close(int fd)
{
    return hj_handle_close(fd) == HjResult::SUCCESS ? 0 : -1;
}

off_t lseek(int fd, off_t offset, int whence)
{
    ssize64_t offset_64 = offset;
    ssize64_t offset_beg;

    HjResult result = hj_handle_seek(fd, &offset_64, (HjWhence)whence, &offset_beg);
    if (result != HjResult::SUCCESS)
    {
        // TODO: set errno
        return (off_t)-1;
    }

    return offset_beg;
}