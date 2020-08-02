#pragma once

#include <libsystem/Common.h>

struct MemoryObject
{
    int id;
    uintptr_t address;
    size_t size;

    int refcount;
};

void memory_object_initialize();

MemoryObject *memory_object_create(size_t size);

void memory_object_destroy(MemoryObject *memory_object);

MemoryObject *memory_object_ref(MemoryObject *memory_object);

void memory_object_deref(MemoryObject *memory_object);

MemoryObject *memory_object_by_id(int id);
