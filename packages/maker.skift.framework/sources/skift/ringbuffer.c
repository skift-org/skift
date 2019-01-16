/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdlib.h>
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

bool ringbuffer_write(ringbuffer_t *rb, byte c)
{
    uint index = (rb->head + 1) % rb->size;

    if (index == rb->tail)
    {
        return false;
    }

    rb->buffer[index] = c;
    rb->head = index;

    return true;
}

bool ringbuffer_read(ringbuffer_t *rb, byte *c)
{
    if (rb->head != rb->tail)
    {
        uint index = (rb->head + 1) % rb->size;

        *c = rb->buffer[index];
        rb->tail = index;

        return true;
    }

    return false;
}