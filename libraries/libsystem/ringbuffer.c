/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* ringbuffer.c: a fifo buffer.                                               */

#include <libsystem/assert.h>
#include <libsystem/ringbuffer.h>

ringbuffer_t *ringbuffer(uint size)
{
    ringbuffer_t *rb = __malloc(ringbuffer_t);

    rb->size = size;
    rb->head = 0;
    rb->tail = 0;

    rb->buffer = malloc(size);

    return rb;
}

void ringbuffer_delete(ringbuffer_t *rb)
{
    assert(rb);

    free(rb->buffer);
    free(rb);
}

int ringbuffer_is_empty(ringbuffer_t *this)
{
    return this->tail == this->head;
}

int ringbuffer_is_full(ringbuffer_t *this)
{
    return ((this->head + 1) % this->size) == this->tail;
}

int ringbuffer_read(ringbuffer_t *rb, void *buffer, uint size)
{
    assert(rb);
    assert(buffer);

    int chr;
    uint offset = 0;

    do
    {
        chr = ringbuffer_getc(rb);

        if (chr != -1)
        {
            ((char *)buffer)[offset] = (char)chr;
            offset++;
        }
    } while (chr != -1 && offset < size);

    return offset;
}

int ringbuffer_write(ringbuffer_t *rb, const void *buffer, uint size)
{
    assert(rb);
    assert(buffer);

    int chr = 0;
    uint offset = 0;

    while (chr != -1 && offset < size)
    {
        chr = ringbuffer_putc(rb, ((char *)buffer)[offset]);
        offset++;
    }

    return offset;
}

int ringbuffer_putc(ringbuffer_t *rb, int c)
{
    assert(rb);

    if (!ringbuffer_is_full(rb))
    {
        rb->buffer[rb->head] = (uchar)c;
        rb->head = (rb->head + 1) % rb->size;

        return (uchar)c;
    }
    else
    {
        return -1;
    }
}

int ringbuffer_getc(ringbuffer_t *rb)
{
    assert(rb);

    if (!ringbuffer_is_empty(rb))
    {
        int c = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % rb->size;

        return c;
    }
    else
    {
        return -1;
    }
}