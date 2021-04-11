
#include <assert.h>
#include <skift/Printf.h>
#include <string.h>

#include <libmath/MinMax.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>
#include <libsystem/io/Stream_internal.h>

Stream *_in_stream = nullptr;
Stream *_out_stream = nullptr;
Stream *_err_stream = nullptr;
Stream *_log_stream = nullptr;

Stream *__stream_get_in_stream()
{
    if (_in_stream == nullptr)
    {
        _in_stream = stream_open_handle(0, OPEN_READ);
    }

    return _in_stream;
}

Stream *__stream_get_out_stream()
{
    if (_out_stream == nullptr)
    {
        _out_stream = stream_open_handle(1, OPEN_WRITE | OPEN_BUFFERED);
    }

    return _out_stream;
}

Stream *__stream_get_err_stream()
{
    if (_err_stream == nullptr)
    {
        _err_stream = stream_open_handle(2, OPEN_WRITE | OPEN_BUFFERED);
    }

    return _err_stream;
}

Stream *__stream_get_log_stream()
{
    if (_log_stream == nullptr)
    {
        _log_stream = stream_open_handle(3, OPEN_WRITE | OPEN_BUFFERED);
    }

    return _log_stream;
}

static void stream_initialize(Stream *stream)
{
    if (handle_has_flags(stream, OPEN_BUFFERED | OPEN_WRITE))
    {
        stream->write_mode = STREAM_BUFFERED_LINE;
        stream->write_buffer = malloc(STREAM_BUFFER_SIZE);
    }

    if (handle_has_flags(stream, OPEN_BUFFERED | OPEN_READ))
    {
        stream->read_mode = STREAM_BUFFERED_BLOCK;
        stream->read_buffer = malloc(STREAM_BUFFER_SIZE);
    }
}

Stream *stream_open(const char *path, OpenFlag flags)
{
    Stream *stream = CREATE(Stream);

    __plug_handle_open(HANDLE(stream), path, flags | OPEN_STREAM);

    stream_initialize(stream);

    return stream;
}

Stream *stream_open_handle(int handle_id, OpenFlag flags)
{
    Stream *stream = CREATE(Stream);

    HANDLE(stream)->id = handle_id;
    HANDLE(stream)->flags = flags | OPEN_STREAM;
    HANDLE(stream)->result = SUCCESS;

    stream_initialize(stream);

    return stream;
}

Result stream_create_term(Stream **server, Stream **client)
{
    *server = nullptr;
    *client = nullptr;

    int server_handle = HANDLE_INVALID_ID;
    int client_handle = HANDLE_INVALID_ID;

    Result result = __plug_create_term(&server_handle, &client_handle);

    if (result == SUCCESS)
    {
        *server = stream_open_handle(server_handle, OPEN_READ | OPEN_WRITE);
        *client = stream_open_handle(client_handle, OPEN_READ | OPEN_WRITE);
    }

    return result;
}

void stream_close(Stream *stream)
{
    stream_flush(stream);

    if (stream->write_buffer)
    {
        free(stream->write_buffer);
    }

    if (stream->read_buffer)
    {
        free(stream->read_buffer);
    }

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

void stream_set_read_buffer_mode(Stream *stream, StreamBufferMode mode)
{
    if (mode == STREAM_BUFFERED_NONE)
    {
        if (stream->read_buffer)
        {
            free(stream->read_buffer);
        }
    }
    else
    {
        if (!stream->read_buffer)
        {
            stream->read_buffer = malloc(STREAM_BUFFER_SIZE);
        }
    }

    stream->read_mode = mode;
}

void stream_set_write_buffer_mode(Stream *stream, StreamBufferMode mode)
{
    stream_flush(stream);

    if (mode == STREAM_BUFFERED_NONE)
    {
        if (stream->write_buffer)
        {
            free(stream->write_buffer);
        }
    }
    else
    {
        if (!stream->write_buffer)
        {
            stream->write_buffer = malloc(STREAM_BUFFER_SIZE);
        }
    }

    stream->write_mode = mode;
}

int stream_fill(Stream *stream)
{

    stream->read_used = __plug_handle_read(HANDLE(stream), stream->read_buffer, STREAM_BUFFER_SIZE);
    stream->read_head = 0;

    return stream->read_used;
}

int stream_read_buffered(Stream *stream, void *buffer, size_t size)
{
    size_t data_left = size;
    char *data_to_read = (char *)buffer;

    while (data_left != 0)
    {
        // Fill the buffer with data
        if (stream->read_head == stream->read_used)
        {
            if (stream_fill(stream) == 0)
            {
                // Look like we have no more data to read
                return size - data_left;
            }
        }

        // How many data can we copy from the buffer
        size_t used_space = stream->read_used - stream->read_head;
        size_t data_added = MIN(used_space, data_left);

        // Copy the data from the buffer
        memcpy(data_to_read, ((char *)stream->read_buffer) + stream->read_head, data_added);

        // Update the amount read
        data_left -= data_added;
        stream->read_head += data_added;
    }

    return size - data_left;
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

    if (stream->read_mode == STREAM_BUFFERED_NONE)
    {
        result = __plug_handle_read(HANDLE(stream), buffer, size);
    }
    else
    {
        result = stream_read_buffered(stream, buffer, size);
    }

    if (result == 0)
    {
        stream->is_end_of_file = true;
    }

    return result;
}

static size_t stream_write_linebuffered(Stream *stream, const void *buffer, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        char c = ((char *)buffer)[i];

        // Append to the buffer
        ((char *)stream->write_buffer)[stream->write_used] = c;
        stream->write_used++;

        // Flush if this is a new line or the end of the buffer
        if (c == '\n' || stream->write_used == STREAM_BUFFER_SIZE)
        {
            stream_flush(stream);
        }
    }

    return size;
}

static size_t stream_write_buffered(Stream *stream, const void *buffer, size_t size)
{
    int data_left = size;

    char *data_to_write = (char *)buffer;

    while (data_left > 0)
    {
        // Append the data to the buffer
        int free_space = STREAM_BUFFER_SIZE - stream->write_used;
        int data_added = MIN(free_space, data_left);

        // Copy the data to the buffer
        memcpy(((char *)(stream->write_buffer)) + stream->write_used, data_to_write, data_added);
        stream->write_used += data_added;
        data_left -= data_added;
        data_to_write += data_added;

        // flush the data if buffer is full
        if (stream->write_used == STREAM_BUFFER_SIZE)
        {
            stream_flush(stream);
        }
    }

    return size;
}

size_t stream_write(Stream *stream, const void *buffer, size_t size)
{
    if (!stream)
    {
        return 0;
    }

    switch (stream->write_mode)
    {
    case STREAM_BUFFERED_NONE:
        return __plug_handle_write(HANDLE(stream), buffer, size);

    case STREAM_BUFFERED_LINE:
        return stream_write_linebuffered(stream, buffer, size);

    case STREAM_BUFFERED_BLOCK:
        return stream_write_buffered(stream, buffer, size);

    default:
        ASSERT_NOT_REACHED();
    }
}

void stream_flush(Stream *stream)
{
    if (!stream)
    {
        return;
    }

    if (stream->write_buffer != nullptr && stream->write_used > 0)
    {
        __plug_handle_write(HANDLE(stream), stream->write_buffer, stream->write_used);
        stream->write_used = 0;
    }
}

Result stream_call(Stream *stream, IOCall request, void *arg)
{
    return __plug_handle_call(HANDLE(stream), request, arg);
}

int stream_seek(Stream *stream, IO::SeekFrom from)
{
    return __plug_handle_seek(HANDLE(stream), from);
}

int stream_tell(Stream *stream)
{
    return __plug_handle_tell(HANDLE(stream));
}

void stream_stat(Stream *stream, FileState *stat)
{
    stat->type = FILE_TYPE_UNKNOWN;
    stat->size = 0;

    __plug_handle_stat(HANDLE(stream), stat);
}

int stream_putchar(Stream *stream, char c)
{
    if (stream_write(stream, &c, sizeof(char)) != sizeof(c))
    {
        return -1;
    }

    return c;
}

char stream_getchar(Stream *stream)
{
    char c;

    if (stream_read(stream, &c, sizeof(char)) != sizeof(char))
    {
        return -1;
    }

    return c;
}

int stream_ungetchar(Stream *stream, char c)
{
    stream->has_unget = true;
    stream->unget_char = c;

    return 0;
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

bool stream_is_end_file(Stream *stream)
{
    return stream->is_end_of_file;
}
