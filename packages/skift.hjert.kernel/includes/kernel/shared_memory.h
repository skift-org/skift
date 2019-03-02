#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

typedef struct 
{
    void * memory;
    uint refcount;
    uint size;
} shared_memory_t;

void shared_memory_setup(void);

shared_memory_t * shared_memory(uint size);
void shared_memory_delete(shared_memory_t * memory);

void* shared_memory_create(uint size);
void* shared_memory_aquire(void* mem);
void  shared_memory_realease(void* mem);