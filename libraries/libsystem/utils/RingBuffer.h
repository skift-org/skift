#pragma once

#include <libsystem/Common.h>

typedef struct
{
    size_t head;
    size_t tail;
    size_t size;
    size_t used;

    char buffer[];
} RingBuffer;

RingBuffer *ringbuffer_create(size_t size);

void ringbuffer_destroy(RingBuffer *ringbuffer);

bool ringbuffer_is_empty(RingBuffer *ringbuffer);

bool ringbuffer_is_full(RingBuffer *ringbuffer);

size_t ringbuffer_used(RingBuffer *ringbuffer);

void ringbuffer_putc(RingBuffer *ringbuffer, char c);

char ringbuffer_getc(RingBuffer *ringbuffer);

char ringbuffer_peek(RingBuffer *ringbuffer, size_t peek);

size_t ringbuffer_read(RingBuffer *ringbuffer, char *buffer, size_t size);

size_t ringbuffer_write(RingBuffer *ringbuffer, const char *buffer, size_t size);
