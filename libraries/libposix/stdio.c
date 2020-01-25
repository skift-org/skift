#include <stdio.h>

#include <libsystem/filesystem.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

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
    TRACE_FUNCTION_BEGIN;

    FILE *fp = (FILE *)stream_open(path, stdio_parse_mode(mode));

    TRACE_FUNCTION_END;

    return fp;
}

int fclose(FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    stream_close((Stream *)stream);

    TRACE_FUNCTION_END;

    return 0;
}

int fflush(FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    stream_flush((Stream *)stream);

    TRACE_FUNCTION_END;

    return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    size_t r = stream_read((Stream *)stream, ptr, size * nmemb);

    TRACE_FUNCTION_END;

    return r;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    size_t r = stream_write((Stream *)stream, ptr, size * nmemb);

    TRACE_FUNCTION_END;

    return r;
}

int fseek(FILE *stream, long offset, int whence)
{
    TRACE_FUNCTION_BEGIN;

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

    TRACE_FUNCTION_END;

    return r;
}

long ftell(FILE *stream)
{
    TRACE_FUNCTION_BEGIN;

    long r = stream_tell((Stream *)stream, WHENCE_START);

    TRACE_FUNCTION_END;

    return r;
}

int puts(const char *s)
{
    TRACE_FUNCTION_BEGIN;

    size_t lenght = 0;

    for (lenght = 0; s[lenght]; lenght++)
    {
    }

    int r = stream_write(out_stream, s, lenght);

    TRACE_FUNCTION_END;

    return r;
}

int putchar(int c)
{
    TRACE_FUNCTION_BEGIN;

    int r = stream_putchar(out_stream, c);

    TRACE_FUNCTION_END;

    return r;
}

int fprintf(FILE *stream, const char *fmt, ...)
{
    TRACE_FUNCTION_BEGIN;

    va_list va;
    va_start(va, fmt);

    int result = stream_vprintf((Stream *)stream, fmt, va);

    va_end(va);

    TRACE_FUNCTION_END;

    return result;
}

int vfprintf(FILE *stream, const char *fmt, va_list va)
{
    TRACE_FUNCTION_BEGIN;

    int r = stream_vprintf((Stream *)stream, fmt, va);

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