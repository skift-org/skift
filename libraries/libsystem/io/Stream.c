/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>

#include <libmath/MinMax.h>
#include <libsystem/__plugs__.h>
#include <libsystem/__printf__.h>
#include <libsystem/assert.h>
#include <libsystem/io/Stream.h>

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
    Stream *stream = __create(Stream);

    __plug_handle_open(HANDLE(stream), path, flags | OPEN_STREAM);

    stream_initialize(stream);

    return stream;
}

Stream *stream_open_handle(int handle_id, OpenFlag flags)
{
    Stream *stream = __create(Stream);

    HANDLE(stream)->id = handle_id;
    HANDLE(stream)->flags = flags | OPEN_STREAM;
    HANDLE(stream)->result = SUCCESS;

    stream_initialize(stream);

    return stream;
}

Result stream_create_pipe(Stream **reader, Stream **writer)
{
    *reader = NULL;
    *writer = NULL;

    int reader_handle = HANDLE_INVALID_ID;
    int writer_handle = HANDLE_INVALID_ID;

    Result result = __plug_create_pipe(&reader_handle, &writer_handle);

    if (result == SUCCESS)
    {
        *reader = stream_open_handle(reader_handle, OPEN_READ);
        *writer = stream_open_handle(writer_handle, OPEN_WRITE);
    }

    return result;
}

Result stream_create_term(Stream **master, Stream **slave)
{
    *master = NULL;
    *slave = NULL;

    int master_handle = HANDLE_INVALID_ID;
    int slave_handle = HANDLE_INVALID_ID;

    Result result = __plug_create_term(&master_handle, &slave_handle);

    if (result == SUCCESS)
    {
        *master = stream_open_handle(master_handle, OPEN_READ | OPEN_WRITE);
        *slave = stream_open_handle(slave_handle, OPEN_READ | OPEN_WRITE);
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

        // Update the amount readed
        data_left -= data_added;
        stream->read_head += data_added;
    }

    return size - data_left;
}

size_t stream_read(Stream *stream, void *buffer, size_t size)
{

    if (!stream)
        return 0;

    if (stream->has_unget && size >= 1)
    {
        ((char *)buffer)[0] = stream->unget_char;
        stream->has_unget = false;
        buffer = &((char *)buffer)[1];
        size--;
    }

    if (stream->write_mode == STREAM_BUFFERED_NONE)
    {
        return __plug_handle_read(HANDLE(stream), buffer, size);
    }
    else
    {
        return stream_read_buffered(stream, buffer, size);
    }
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
        return 0;

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
        return;

    if (stream->write_buffer != NULL && stream->write_used > 0)
    {
        __plug_handle_write(HANDLE(stream), stream->write_buffer, stream->write_used);
        stream->write_used = 0;
    }
}

Result stream_call(Stream *stream, int request, void *arg)
{
    return __plug_handle_call(HANDLE(stream), request, arg);
}

int stream_seek(Stream *stream, int offset, Whence whence)
{
    return __plug_handle_seek(HANDLE(stream), offset, whence);
}

int stream_tell(Stream *stream, Whence whence)
{
    return __plug_handle_tell(HANDLE(stream), whence);
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

    return (int)(uchar)c;
}

char stream_getchar(Stream *stream)
{
    char c;

    if (stream_read(stream, &c, sizeof(char)) != sizeof(char))
    {
        return -1;
    }

    return (int)(uchar)c;
}

int stream_ungetchar(Stream *stream, char c)
{

    stream->has_unget = true;
    stream->unget_char = c;

    return 0;
}

void stream_printf_append(printf_info_t *info, char c)
{
    stream_write((Stream *)info->output, &c, 1);
}

int stream_printf(Stream *stream, const char *fmt, ...)
{

    va_list va;
    va_start(va, fmt);

    int result = stream_vprintf(stream, fmt, va);

    va_end(va);

    return result;
}

int stream_vprintf(Stream *stream, const char *fmt, va_list va)
{
    printf_info_t info = (printf_info_t){
        .format = fmt,
        .append = stream_printf_append,
        .output = (void *)stream,
        .allocated = -1,
    };

    return __printf(&info, va);
}
