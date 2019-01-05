#pragma once

#include <skift/generic.h>

typedef struct 
{
    void * memory;
    uint refcount;
    uint size;
} shared_memory_t;

shared_memory_t * shared_memory(uint size);
void shared_memory_delete(shared_memory_t * memory);

void* shared_memory_create(uint size);
void* shared_memory_aquire(void* mem);
void  shared_memory_realease(void* mem);