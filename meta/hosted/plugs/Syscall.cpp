#include <abi/Syscalls.h>
#include <libmath/MinMax.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int open_flags_to_posix(OpenFlag flags)
{
    int result = 0;

    if ((flags & OPEN_WRITE) && (flags & OPEN_READ))
    {
        result |= O_RDWR;
    }
    else
    {
        result |= O_RDONLY;
    }

    if (flags & OPEN_CREATE)
    {
        result |= O_CREAT;
    }

    if (flags & OPEN_APPEND)
    {
        result |= O_APPEND;
    }

    if (flags & OPEN_TRUNC)
    {
        result |= O_TRUNC;
    }

    if (flags & OPEN_BUFFERED)
    {
        // TODO
    }

    if (flags & OPEN_STREAM)
    {
        // TODO
    }

    if (flags & OPEN_DIRECTORY)
    {
        // TODO
    }

    if (flags & OPEN_SOCKET)
    {
        // TODO
    }

    if (flags & OPEN_CLIENT)
    {
        // TODO
    }

    if (flags & OPEN_SERVER)
    {
        // TODO
    }

    return result;
}

int whence_to_posix(HjWhence whence)
{
    if (whence == HJ_WHENCE_START)
    {
        return SEEK_SET;
    }

    if (whence == HJ_WHENCE_CURRENT)
    {
        return SEEK_CUR;
    }

    if (whence == HJ_WHENCE_END)
    {
        return SEEK_END;
    }

    return -1;
}

FileType mode_to_skift_file_type(mode_t mode)
{
    if (S_ISDIR(mode))
    {
        return FILE_TYPE_DIRECTORY;
    }

    if (S_ISFIFO(mode))
    {
        return FILE_TYPE_PIPE;
    }

    return FILE_TYPE_REGULAR;
}

FileState stat_to_skift(struct stat sb)
{
    FileState result;

    result.size = sb.st_size;
    result.type = mode_to_skift_file_type(sb.st_mode);

    return result;
}

Result errno_to_skift_result()
{
    Result result = SUCCESS;

    if (errno != 0 && result == SUCCESS)
    {
        // Catch all
        return ERR_UNKNOWN;
    }

    errno = 0;

    return result;
}

Result hj_handle_open(int *handle, const char *raw_path, size_t size, OpenFlag flags)
{
    char buffer[256];
    strlcpy(buffer, raw_path, MIN(256, size + 1));
    open(buffer, open_flags_to_posix(flags));

    return errno_to_skift_result();
}

Result hj_handle_close(int handle)
{
    close(handle);

    return errno_to_skift_result();
}

Result hj_handle_read(int handle, void *buffer, size_t size, size_t *amount_read)
{
    *amount_read = read(handle, buffer, size);

    return errno_to_skift_result();
}

Result hj_handle_write(int handle, const void *buffer, size_t size, size_t *amount_written)
{
    *amount_written = write(handle, buffer, size);

    return errno_to_skift_result();
}

Result hj_handle_seek(int handle, ssize64_t *offset, HjWhence whence, ssize64_t *result)
{
    *result = lseek(handle, *offset, whence_to_posix(whence));

    return errno_to_skift_result();
}

Result hj_handle_stat(int handle, FileState *state)
{
    struct stat sb;
    fstat(handle, &sb);

    *state = stat_to_skift(sb);

    return errno_to_skift_result();
}
