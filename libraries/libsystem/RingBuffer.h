#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

typedef struct
{
    size_t size;
    size_t head;
    size_t tail;

    uchar *buffer;
} RingBuffer;

RingBuffer *ringbuffer_create(size_t size);

void ringbuffer_destroy(RingBuffer *rb);

bool ringbuffer_is_empty(RingBuffer *this);

bool ringbuffer_is_full(RingBuffer *this);

size_t ringbuffer_read(RingBuffer *rb, void *buffer, size_t size);

size_t ringbuffer_write(RingBuffer *rb, const void *buffer, size_t size);

int ringbuffer_putc(RingBuffer *rb, int c);

int ringbuffer_getc(RingBuffer *rb);
