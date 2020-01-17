/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* ringbuffer.c: a fifo buffer.                                               */

#include <libsystem/RingBuffer.h>
#include <libsystem/assert.h>

RingBuffer *ringbuffer_create(size_t size)
{
    RingBuffer *rb = __create(RingBuffer);

    rb->size = size;
    rb->head = 0;
    rb->tail = 0;

    rb->buffer = malloc(size);

    return rb;
}

void ringbuffer_destroy(RingBuffer *rb)
{
    assert(rb);

    free(rb->buffer);
    free(rb);
}

bool ringbuffer_is_empty(RingBuffer *this)
{
    return this->tail == this->head;
}

bool ringbuffer_is_full(RingBuffer *this)
{
    return ((this->head + 1) % this->size) == this->tail;
}

size_t ringbuffer_read(RingBuffer *rb, void *buffer, size_t size)
{
    assert(rb);
    assert(buffer);

    int chr;
    size_t offset = 0;

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

size_t ringbuffer_write(RingBuffer *rb, const void *buffer, size_t size)
{
    assert(rb);
    assert(buffer);

    int chr = 0;
    size_t offset = 0;

    while (chr != -1 && offset < size)
    {
        chr = ringbuffer_putc(rb, ((char *)buffer)[offset]);
        offset++;
    }

    return offset;
}

int ringbuffer_putc(RingBuffer *rb, int c)
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

int ringbuffer_getc(RingBuffer *rb)
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