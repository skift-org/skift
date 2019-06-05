/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* ringbuffer.c: a fifo buffer.                                               */

#include <skift/assert.h>
#include <skift/ringbuffer.h>

ringbuffer_t *ringbuffer(uint size)
{
    ringbuffer_t *rb = MALLOC(ringbuffer_t);

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

    int chr;
    uint offset = 0;

    do
    {
        chr = ringbuffer_putc(rb, ((char *)buffer)[offset]);
        offset++;
    } while (chr != -1 && offset < size);

    return offset;
}

int ringbuffer_putc(ringbuffer_t *rb, int c)
{
    assert(rb);

    if (rb->head + 1 != rb->tail)
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

    if (rb->tail != rb->head)
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