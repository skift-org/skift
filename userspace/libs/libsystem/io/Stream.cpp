
#include <assert.h>
#include <skift/Printf.h>
#include <string.h>

#include <libmath/MinMax.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>

Stream *stream_open(const char *path, HjOpenFlag flags)
{
    Stream *stream = CREATE(Stream);

    __plug_handle_open(HANDLE(stream), path, flags | HJ_OPEN_STREAM);

    return stream;
}

Stream *stream_open_handle(int handle_id, HjOpenFlag flags)
{
    Stream *stream = CREATE(Stream);

    HANDLE(stream)->id = handle_id;
    HANDLE(stream)->flags = flags | HJ_OPEN_STREAM;
    HANDLE(stream)->result = SUCCESS;

    return stream;
}

void stream_close(Stream *stream)
{
    __plug_handle_close(HANDLE(stream));

    free(stream);
}

void stream_cleanup(Stream **stream)
{
    if (*stream)
    {
        stream_close(*stream);
        *stream = nullptr;
    }
}

size_t stream_read(Stream *stream, void *buffer, size_t size)
{
    if (!stream)
    {
        return 0;
    }

    size_t result = 0;

    if (stream->has_unget && size >= 1)
    {
        ((char *)buffer)[0] = stream->unget_char;
        stream->has_unget = false;
        buffer = &((char *)buffer)[1];
        size--;

        result = 1;
    }

    result = __plug_handle_read(HANDLE(stream), buffer, size);

    if (result == 0)
    {
        stream->is_end_of_file = true;
    }

    return result;
}

size_t stream_write(Stream *stream, const void *buffer, size_t size)
{
    if (!stream)
    {
        return 0;
    }

    return __plug_handle_write(HANDLE(stream), buffer, size);
}

HjResult stream_call(Stream *stream, IOCall request, void *arg)
{
    return __plug_handle_call(HANDLE(stream), request, arg);
}

int stream_seek(Stream *stream, IO::SeekFrom from)
{
    return __plug_handle_seek(HANDLE(stream), from);
}

void stream_stat(Stream *stream, HjStat *stat)
{
    stat->type = HJ_FILE_TYPE_UNKNOWN;
    stat->size = 0;

    __plug_handle_stat(HANDLE(stream), stat);
}

void stream_format_append(printf_info_t *info, char c)
{
    stream_write((Stream *)info->output, &c, 1);
}

int stream_format(Stream *stream, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = stream_vprintf(stream, fmt, va);

    va_end(va);

    return result;
}

int stream_vprintf(Stream *stream, const char *fmt, va_list va)
{
    printf_info_t info = {};

    info.format = fmt;
    info.append = stream_format_append;
    info.output = (void *)stream;
    info.allocated = -1;

    return __printf(&info, va);
}
