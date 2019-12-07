#include <stdio.h>

#include <libsystem/iostream.h>
#include <libsystem/logger.h>
#include <libsystem/filesystem.h>

#undef printf
#undef puts

FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

__attribute__((constructor)) void stdio_initialize(void)
{
    TRACE_FUNCTION_BEGIN;

    stdin = in_stream;
    stdout = out_stream;
    stderr = log_stream;

    TRACE_FUNCTION_END;
}

IOStreamFlag stdio_parse_mode(const char *mode)
{
    IOStreamFlag flags = 0;

    for (int i = 0; mode[i]; i++)
    {
        char c = mode[i];

        if (c == 'r')
        {
            flags |= IOSTREAM_READ;
        }
        else if (c == 'w')
        {
            flags |= IOSTREAM_WRITE;
            flags |= IOSTREAM_CREATE;
            flags |= IOSTREAM_TRUNC;
        }
        else if (c == 'a')
        {
            flags |= IOSTREAM_WRITE;
            flags |= IOSTREAM_CREATE;
            flags |= IOSTREAM_APPEND;
        }
        else if (c == '+')
        {
            flags |= IOSTREAM_READ;
        }
    }

    return flags;
}

FILE *fopen(const char *path, const char *mode)
{
    TRACE_FUNCTION_BEGIN;

    FILE *fp = (FILE *)iostream_open(path, stdio_parse_mode(mode));

    TRACE_FUNCTION_END;

    return fp;
}

int fclose(FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    iostream_close((IOStream *)stream);

    TRACE_FUNCTION_END;

    return 0;
}

int fflush(FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    int r = iostream_flush((IOStream *)stream);

    TRACE_FUNCTION_END;

    return r;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    size_t r = iostream_read((IOStream *)stream, ptr, size * nmemb);

    TRACE_FUNCTION_END;

    return r;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    size_t r = iostream_write((IOStream *)stream, ptr, size * nmemb);

    TRACE_FUNCTION_END;

    return r;
}

int fseek(FILE *stream, long offset, int whence)
{
    TRACE_FUNCTION_BEGIN;

    int r = -1;

    if (whence == SEEK_SET)
    {
        r = iostream_seek((IOStream *)stream, offset, IOSTREAM_WHENCE_START);
    }
    else if (whence == SEEK_CUR)
    {
        r = iostream_seek((IOStream *)stream, offset, IOSTREAM_WHENCE_HERE);
    }
    else if (whence == SEEK_END)
    {
        r = iostream_seek((IOStream *)stream, offset, IOSTREAM_WHENCE_END);
    }

    TRACE_FUNCTION_END;

    return r;
}

long ftell(FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    long r = iostream_tell((IOStream *)stream, IOSTREAM_WHENCE_START);

    TRACE_FUNCTION_END;

    return r;
}

int puts(const char *s)
{
    TRACE_FUNCTION_BEGIN;

    int r = iostream_puts(out_stream, s);

    TRACE_FUNCTION_END;

    return r;
}

int putchar(int c)
{
    TRACE_FUNCTION_BEGIN;

    int r = iostream_putchar(out_stream, c);

    TRACE_FUNCTION_END;

    return r;
}

int printf(const char *fmt, ...)
{
    TRACE_FUNCTION_BEGIN;

    va_list va;
    va_start(va, fmt);

    int result = iostream_vprintf(out_stream, fmt, va);

    va_end(va);

    TRACE_FUNCTION_END;

    return result;
}

int fprintf(FILE *stream, const char *fmt, ...)
{
    TRACE_FUNCTION_BEGIN;

    va_list va;
    va_start(va, fmt);

    int result = iostream_vprintf((IOStream *)stream, fmt, va);

    va_end(va);

    TRACE_FUNCTION_END;

    return result;
}

int vfprintf(FILE *stream, const char *fmt, va_list va)
{
    TRACE_FUNCTION_BEGIN;

    int r = iostream_vprintf((IOStream *)stream, fmt, va);

    TRACE_FUNCTION_END;

    return r;
}

int remove(const char *pathname)
{
    TRACE_FUNCTION_BEGIN;

    int r = filesystem_unlink(pathname);

    TRACE_FUNCTION_END;

    return r;
}

int rename(const char *oldpath, const char *newpath)
{
    TRACE_FUNCTION_BEGIN;

    int r = filesystem_rename(oldpath, newpath);

    TRACE_FUNCTION_END;

    return r;
}