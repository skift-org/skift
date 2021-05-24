#include <libutils/List.h>

#include "system/interrupts/Interupts.h"
#include "system/memory/Memory.h"
#include "system/memory/MemoryObject.h"
#include "system/memory/Physical.h"

static int _memory_object_id = 0;
static List<MemoryObject *> *_memory_objects;

void memory_object_initialize()
{
    _memory_objects = new List<MemoryObject *>();
}

MemoryObject *memory_object_create(size_t size)
{
    InterruptsRetainer retainer;

    size = PAGE_ALIGN_UP(size);

    MemoryObject *memory_object = CREATE(MemoryObject);

    memory_object->id = _memory_object_id++;
    memory_object->refcount = 1;
    memory_object->_range = physical_alloc(size);

    _memory_objects->push_back(memory_object);

    return memory_object;
}

void memory_object_destroy(MemoryObject *memory_object)
{
    _memory_objects->remove(memory_object);

    physical_free(memory_object->range());
    free(memory_object);
}

MemoryObject *memory_object_ref(MemoryObject *memory_object)
{
    __atomic_add_fetch(&memory_object->refcount, 1, __ATOMIC_SEQ_CST);

    return memory_object;
}

void memory_object_deref(MemoryObject *memory_object)
{
    InterruptsRetainer retainer;

    if (__atomic_sub_fetch(&memory_object->refcount, 1, __ATOMIC_SEQ_CST) == 0)
    {
        memory_object_destroy(memory_object);
    }
}

MemoryObject *memory_object_by_id(int id)
{
    InterruptsRetainer retainer;

    for (auto *object : *_memory_objects)
    {
        if (object->id == id)
        {
            memory_object_ref(object);
            return object;
        }
    }

    return nullptr;
}
