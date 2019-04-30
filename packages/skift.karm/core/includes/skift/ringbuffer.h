#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

typedef struct
{
    uint size;
    uint head;
    uint tail;

    uchar *buffer;
} ringbuffer_t;

ringbuffer_t *ringbuffer(uint size);
void ringbuffer_delete(ringbuffer_t *rb);

int ringbuffer_read(ringbuffer_t *rb, void* buffer, uint size);
int ringbuffer_write(ringbuffer_t *rb, const void* buffer, uint size);

int ringbuffer_putc(ringbuffer_t *rb, int c);
int ringbuffer_getc(ringbuffer_t *rb);
