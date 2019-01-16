#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/types.h>
#include <skift/utils.h>

typedef struct
{
    uint size;
    uint head;
    uint tail;

    char *buffer;
} ringbuffer_t;

ringbuffer_t *ringbuffer(uint size);
void ringbuffer_delete(ringbuffer_t *rb);

bool ringbuffer_write(ringbuffer_t *rb, byte c);
bool ringbuffer_read(ringbuffer_t *rb, byte *c);
