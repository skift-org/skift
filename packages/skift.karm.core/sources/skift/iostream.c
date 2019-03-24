/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include <skift/iostream.h>

iostream_t *iostream(
    iostream_flag_t flags,

    iostream_read_t *read,
    iostream_write_t *write,
    iostream_tell_t *tell,
    iostream_seek_t *seek,
    iostream_fstat_t *stat,
    iostream_ioctl_t *ioctl,
    iostream_close_t *close,

    void *p)
{
    iostream_t* stream = MALLOC(iostream_t);

    stream->read = read;
    stream->write = write;
    stream->tell = tell;
    stream->seek = seek;
    stream->fstat = stat;
    stream->ioctl = ioctl;
    stream->close = close;

    stream->p = p;

    if (flags & IOSTREAM_BUFFERED_WRITE)
    {
        stream->write_buffer = malloc(IOSTREAM_BUFFER_SIZE);
        stream->write_head = 0;
    }
    else
    {
        stream->write_buffer = NULL;
        stream->write_head = 0;
    }

    if (flags & IOSTREAM_BUFFERED_READ)
    {

    }
    else
    {

    }

    stream->flags = flags;

    return stream;
}

void iostream_delete(iostream_t *stream)
{
    if (stream->close != NULL)
    {
        stream->close(stream);
    }
    
    free(iostream);
}

int iostream_read(iostream_t *stream, void *buffer, uint size)
{
    if (stream->read != NULL)
    {
        return stream->read(stream, buffer, size);
    }

    return -1;
}

static int iostream_write_buffered(iostream_t *stream, void *buffer, uint size)
{
    int data_left = size;

    byte* data_to_write = (byte*)buffer;

    while (data_left > 0)
    {
        // Append the data to the buffer
        int free_space = IOSTREAM_BUFFER_SIZE - stream->write_head;

        memcpy(((byte*)(stream->write_buffer)) + stream->write_head, data_to_write, free_space);
        stream->write_head += free_space;
        data_left -= free_space;
        data_to_write += free_space;
        
        // flush the data if buffer is full
        if (stream->write_head == IOSTREAM_BUFFER_SIZE)
        {
            stream->write(stream, stream->write_buffer, IOSTREAM_BUFFER_SIZE);
        }
    }

    return size;
}

int iostream_write(iostream_t *stream, void *buffer, uint size)
{
    if (stream->write != NULL)
    {
        if (stream->write_buffer != NULL)
        {
            return iostream_write_buffered(stream, buffer, size);
        }
        else
        {
            return stream->write(stream, buffer, size);
        }
    }

    return -1;
}

int iostream_flush(iostream_t *stream)
{
    if (stream->write_buffer != NULL )
    {
        int writed = stream->write_head;

        stream->write(stream, stream->write_buffer, stream->write_head);
        stream->write_head = 0;

        return writed;
    }

    return -1;
}

int iostream_tell(iostream_t *stream, iostream_whence_t whence)
{
    if (stream->tell != NULL)
    {
        return stream->tell(stream, whence);
    }

    return 0;
}

int iostream_seek(iostream_t *stream, int offset, iostream_whence_t whence)
{
    if (stream->seek != NULL)
    {
        return stream->seek(stream, offset, whence);
    }

    return 0;
}

int iostream_fstat(iostream_t *stream, iostream_stat_t* stat)
{
    if (stream->fstat != NULL)
    {
        return stream->fstat(stream, stat);
    }

    return 0;
}

int iostream_ioctl(iostream_t *stream, int request, void *arg)
{
    if (stream->ioctl != NULL)
    {
        return stream->ioctl(stream, request, arg);
    }

    return 0;
}

// TODO: int iostream_printf(iostream_t *stream, const char *fmt, ...)
//       {
//       
//       }

// TODO: int iostream_scanf(iostream_t *stream, const char *fmt, ...)
//       {
//       
//       }
