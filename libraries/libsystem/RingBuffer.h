#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

typedef struct
{
    size_t buffer_allocated;
    size_t buffer_used;

    size_t head;
    size_t tail;

    char buffer[];
} RingBuffer;

RingBuffer *ringbuffer_create(size_t size);

void ringbuffer_destroy(RingBuffer *ringbuffer);

bool ringbuffer_is_empty(RingBuffer *ringbuffer);

bool ringbuffer_is_full(RingBuffer *ringbuffer);

void ringbuffer_putc(RingBuffer *ringbuffer, char c);

char ringbuffer_getc(RingBuffer *ringbuffer);

size_t ringbuffer_read(RingBuffer *ringbuffer, char *buffer, size_t size);

size_t ringbuffer_write(RingBuffer *ringbuffer, const char *buffer, size_t size);
