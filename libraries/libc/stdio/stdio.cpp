#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <abi/Syscalls.h>

#ifndef __KERNEL__

FILE _stdin{0, 0, 0};
FILE _stdout{1, 0, 0};
FILE _stderr{2, 0, 0};
FILE _stdlog{3, 0, 0};

FILE *__stdio_get_stdin()
{
    return &_stdin;
}

FILE *__stdio_get_stdout()
{
    return &_stdout;
}

FILE *__stdio_get_stderr()
{
    return &_stderr;
}

FILE *__stdio_get_stdlog()
{
    return &_stdlog;
}

OpenFlag stdio_parse_mode(const char *mode)
{
    OpenFlag flags = 0;

    for (int i = 0; mode[i]; i++)
    {
        char c = mode[i];

        if (c == 'r')
        {
            flags |= OPEN_READ;
        }
        else if (c == 'w')
        {
            flags |= OPEN_WRITE;
            flags |= OPEN_CREATE;
            flags |= OPEN_TRUNC;
        }
        else if (c == 'a')
        {
            flags |= OPEN_WRITE;
            flags |= OPEN_CREATE;
            flags |= OPEN_APPEND;
        }
        else if (c == '+')
        {
            flags |= OPEN_READ;
        }
    }

    return flags;
}

FILE *fdopen(int fd, const char *mode)
{
    UNUSED(mode);

    FILE *result = (FILE *)malloc(sizeof(FILE));
    result->error = 0;
    result->is_eof = 0;
    result->handle = fd;

    return result;
}

FILE *fopen(const char *path, const char *mode)
{
    OpenFlag flags = stdio_parse_mode(mode);

    int handle = 0;
    Result result = hj_handle_open(&handle, path, strlen(path), flags);

    if (result != Result::SUCCESS)
    {
        // TODO: set errno
        return NULL;
    }

    FILE *file = (FILE *)malloc(sizeof(FILE));
    file->error = 0;
    file->is_eof = 0;
    file->handle = handle;

    return file;
}

int fclose(FILE *file)
{
    Result result = hj_handle_close(file->handle);
    free(file);

    return result == Result::SUCCESS ? 0 : -1;
}

int fflush(FILE *file)
{
    UNUSED(file);

    return 0;
}

size_t fread(void *ptr, size_t size, size_t count, FILE *file)
{
    size_t total = 0;
    Result result;
    uint8_t *dst_ptr = (uint8_t *)ptr;

    for (size_t i = 0; i < count; i++)
    {
        size_t read = 0;
        result = hj_handle_read(file->handle, dst_ptr, size, &read);
        total += read;
        dst_ptr += read;
        if (result != Result::SUCCESS)
        {
            // TODO: set errno
            return total;
        }
    }

    if (total == 0)
    {
        file->is_eof = 1;
    }

    return total;
}

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *file)
{
    size_t total = 0;
    Result result;
    const uint8_t *dst_ptr = (const uint8_t *)ptr;

    for (size_t i = 0; i < count; i++)
    {
        size_t written = 0;
        result = hj_handle_write(file->handle, dst_ptr, size, &written);
        total += written;
        dst_ptr += written;
        if (result != Result::SUCCESS)
        {
            // TODO: set errno
            return total;
        }
    }

    return total;
}

int fseek(FILE *file, long offset, int whence)
{
    Result r = Result::ERR_NOT_IMPLEMENTED;

    ssize64_t offset64 = offset;

    if (whence == SEEK_SET)
    {
        r = hj_handle_seek(file->handle, &offset64, HJ_WHENCE_START, NULL);
    }
    else if (whence == SEEK_CUR)
    {
        r = hj_handle_seek(file->handle, &offset64, HJ_WHENCE_CURRENT, NULL);
    }
    else if (whence == SEEK_END)
    {
        r = hj_handle_seek(file->handle, &offset64, HJ_WHENCE_END, NULL);
    }

    return r == Result::SUCCESS ? 0 : -1;
}

long ftell(FILE *stream)
{
    ssize64_t offset = 0;
    Result r = hj_handle_seek(stream->handle, 0, HJ_WHENCE_CURRENT, &offset);

    if (r != Result::SUCCESS)
    {
        // TODO: check error
    }

    return (long)offset;
}

int puts(const char *s)
{
    int r = fwrite(s, strlen(s), 1, stdout);
    r += fwrite("\n", 1, 1, stdout);

    return r;
}

int getc(FILE *file)
{
    uint8_t c;
    fread(&c, 1, 1, file);
    return (int)c;
}

int putc(int c, FILE *file)
{
    uint8_t b = c;
    fwrite(&b, 1, 1, file);
    return c;
}

int putchar(int c)
{
    int r = puts((const char *)&c);

    return r;
}

int printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int result = vfprintf(__stdio_get_stdout(), fmt, ap);
    va_end(ap);

    return result;
}

int fprintf(FILE *file, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int result = vfprintf(file, fmt, ap);
    va_end(ap);

    return result;
}

int remove(const char *pathname)
{
    return unlink(pathname);
}

int rename(const char *oldpath, const char *newpath)
{
    Result r = hj_filesystem_rename(oldpath, strlen(oldpath), newpath, strlen(newpath));

    return r == Result::SUCCESS ? 0 : -1;
}

int fputs(const char *string, FILE *stream)
{
    return fwrite(string, strlen(string), 1, stream);
}

void clearerr(FILE *stream)
{
    stream->error = 0;
}

int feof(FILE *stream)
{
    return stream->is_eof;
}

int ferror(FILE *stream)
{
    return stream->error;
}

char *fgets(char *s, int size, FILE *file)
{
    fread(s, size, 1, file);

    // TODO: error handling
    return s;
}

int fgetc(FILE *file)
{
    return getc(file);
}

int fputc(int c, FILE *file)
{
    return putc(c, file);
}

int getchar(void)
{
    return fgetc(stdin);
}

int fscanf(FILE *stream, const char *format, ...)
{
    UNUSED(stream);
    UNUSED(format);

    ASSERT_NOT_REACHED();

    return 0;
}

int sscanf(const char *str, const char *format, ...)
{
    UNUSED(str);
    UNUSED(format);

    ASSERT_NOT_REACHED();

    return 0;
}

char *strerror(int e)
{
    const char *s;
    switch (e)
    {
    case EAGAIN:
        s = "Operation would block (EAGAIN)";
        break;
    case EACCES:
        s = "Access denied (EACCESS)";
        break;
    case EBADF:
        s = "Bad file descriptor (EBADF)";
        break;
    case EEXIST:
        s = "File exists already (EEXIST)";
        break;
    case EFAULT:
        s = "Access violation (EFAULT)";
        break;
    case EINTR:
        s = "Operation interrupted (EINTR)";
        break;
    case EINVAL:
        s = "Invalid argument (EINVAL)";
        break;
    case EIO:
        s = "I/O error (EIO)";
        break;
    case EISDIR:
        s = "Resource is directory (EISDIR)";
        break;
    case ENOENT:
        s = "No such file or directory (ENOENT)";
        break;
    case ENOMEM:
        s = "Out of memory (ENOMEM)";
        break;
    case ENOTDIR:
        s = "Expected directory instead of file (ENOTDIR)";
        break;
    case ENOSYS:
        s = "Operation not implemented (ENOSYS)";
        break;
    case EPERM:
        s = "Operation not permitted (EPERM)";
        break;
    case EPIPE:
        s = "Broken pipe (EPIPE)";
        break;
    case ESPIPE:
        s = "Seek not possible (ESPIPE)";
        break;
    case ENXIO:
        s = "No such device or address (ENXIO)";
        break;
    case ENOEXEC:
        s = "Exec format error (ENOEXEC)";
        break;
    default:
        s = "Unknown error code (?)";
    }
    return const_cast<char *>(s);
}

void perror(const char *string)
{
    int error = errno;
    if (string && *string)
    {
        fprintf(stderr, "%s: ", string);
    }
    fprintf(stderr, "%s\n", strerror(error));
}

int setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
    UNUSED(stream);
    UNUSED(buf);
    UNUSED(mode);
    UNUSED(size);
    // TODO: implement this

    ASSERT_NOT_REACHED();

    return -1;
}

#endif