/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* ringbuffer.c: a fifo buffer.                                               */


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
    free(rb->buffer);
    free(rb);
}

int ringbuffer_read(ringbuffer_t *rb, void* buffer, uint size)
{
    int chr;
    uint offset = 0;

    do 
    {
        chr = ringbuffer_getc(rb);

        if (chr != -1)
        {
            ((char*)buffer)[offset] = (char)chr;
            offset++;
        }
    }
    while(chr != -1 && offset < size);

    return offset;
}

int ringbuffer_write(ringbuffer_t *rb, const void* buffer, uint size)
{
    int chr;
    uint offset = 0;

    do 
    {
        chr = ringbuffer_putc(rb, ((char*)buffer)[offset]);
        offset++;
    }
    while(chr != -1 && offset < size);

    return offset;  
}

int ringbuffer_putc(ringbuffer_t *rb, int c)
{
    uint index = (rb->head + 1) % rb->size;

    if (index == rb->tail)
    {
        return -1;
    }

    rb->buffer[index] = (uchar)c;
    rb->head = index;

    return (uchar)c;
}

int ringbuffer_getc(ringbuffer_t *rb)
{
    if (rb->head != rb->tail)
    {
        int c = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % rb->size;

        return c;
    }

    return -1;
}