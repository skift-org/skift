/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/RingBuffer.h>
#include <libsystem/assert.h>

RingBuffer *ringbuffer_create(size_t size)
{
    RingBuffer *ringbuffer = calloc(1, sizeof(RingBuffer) + size);

    ringbuffer->buffer_allocated = size;

    return ringbuffer;
}

void ringbuffer_destroy(RingBuffer *ringbuffer)
{
    free(ringbuffer);
}

bool ringbuffer_is_empty(RingBuffer *ringbuffer)
{
    return ringbuffer->buffer_used == 0;
}

bool ringbuffer_is_full(RingBuffer *ringbuffer)
{
    return ringbuffer->buffer_used == ringbuffer->buffer_allocated;
}

void ringbuffer_putc(RingBuffer *ringbuffer, char c)
{
    assert(!ringbuffer_is_full(ringbuffer));

    ringbuffer->buffer[ringbuffer->head] = c;
    ringbuffer->head = (ringbuffer->head + 1) % (ringbuffer->buffer_allocated);
    ringbuffer->buffer_used++;
}

char ringbuffer_getc(RingBuffer *ringbuffer)
{
    assert(!ringbuffer_is_empty(ringbuffer));

    char c = ringbuffer->buffer[ringbuffer->tail];
    ringbuffer->tail = (ringbuffer->tail + 1) % (ringbuffer->buffer_allocated);
    ringbuffer->buffer_used--;

    return c;
}

size_t ringbuffer_read(RingBuffer *ringbuffer, char *buffer, size_t size)
{
    size_t readed = 0;

    while (!ringbuffer_is_empty(ringbuffer) && readed < size)
    {
        buffer[readed] = ringbuffer_getc(ringbuffer);
        readed++;
    }

    return readed;
}

size_t ringbuffer_write(RingBuffer *ringbuffer, const char *buffer, size_t size)
{
    size_t written = 0;

    while (!ringbuffer_is_full(ringbuffer) && written < size)
    {
        ringbuffer_putc(ringbuffer, buffer[written]);
        written++;
    }

    return written;
}