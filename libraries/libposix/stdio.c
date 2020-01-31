#include <stdio.h>

#include <libsystem/filesystem.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

#undef printf
#undef puts

FILE *__stdio_get_stdin(void)
{
    return in_stream;
}

FILE *__stdio_get_stdout(void)
{
    return in_stream;
}

FILE *__stdio_get_stderr(void)
{
    return in_stream;
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

FILE *fopen(const char *path, const char *mode)
{
    logger_trace("fopen(%s, %s)", path, mode);

    Stream *stream = stream_open(path, stdio_parse_mode(mode));

    if (handle_has_error(stream))
    {
        stream_close(stream);

        return NULL;
    }

    return stream;
}

int fclose(FILE *stream)
{
    stream_close((Stream *)stream);

    return 0;
}

int fflush(FILE *stream)
{
    stream_flush((Stream *)stream);

    return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t r = stream_read((Stream *)stream, ptr, size * nmemb);

    return r;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t r = stream_write((Stream *)stream, ptr, size * nmemb);

    return r;
}

int fseek(FILE *stream, long offset, int whence)
{

    int r = -1;

    if (whence == SEEK_SET)
    {
        r = stream_seek((Stream *)stream, offset, WHENCE_START);
    }
    else if (whence == SEEK_CUR)
    {
        r = stream_seek((Stream *)stream, offset, WHENCE_HERE);
    }
    else if (whence == SEEK_END)
    {
        r = stream_seek((Stream *)stream, offset, WHENCE_END);
    }

    return r;
}

long ftell(FILE *stream)
{

    long r = stream_tell((Stream *)stream, WHENCE_START);

    return r;
}

int puts(const char *s)
{

    size_t lenght = 0;

    for (lenght = 0; s[lenght]; lenght++)
    {
    }

    int r = stream_write(out_stream, s, lenght);
    r += stream_write(out_stream, "\n", 1);

    return r;
}

int putchar(int c)
{
    int r = stream_putchar(out_stream, c);

    return r;
}

int printf(const char *fmt, ...)
{

    va_list va;
    va_start(va, fmt);

    int result = stream_vprintf(out_stream, fmt, va);

    va_end(va);

    return result;
}

int fprintf(FILE *stream, const char *fmt, ...)
{

    va_list va;
    va_start(va, fmt);

    int result = stream_vprintf((Stream *)stream, fmt, va);

    va_end(va);

    return result;
}

int vfprintf(FILE *stream, const char *fmt, va_list va)
{

    int r = stream_vprintf((Stream *)stream, fmt, va);

    return r;
}

int remove(const char *pathname)
{

    int r = filesystem_unlink(pathname);

    return r;
}

int rename(const char *oldpath, const char *newpath)
{
    int r = filesystem_rename(oldpath, newpath);

    return r;
}