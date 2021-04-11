#include <libsystem/utils/List.h>

#include "kernel/interrupts/Interupts.h"
#include "kernel/memory/Memory.h"
#include "kernel/memory/MemoryObject.h"
#include "kernel/memory/Physical.h"

static int _memory_object_id = 0;
static List *_memory_objects;

void memory_object_initialize()
{
    _memory_objects = list_create();
}

MemoryObject *memory_object_create(size_t size)
{
    InterruptsRetainer retainer;

    size = PAGE_ALIGN_UP(size);

    MemoryObject *memory_object = CREATE(MemoryObject);

    memory_object->id = _memory_object_id++;
    memory_object->refcount = 1;
    memory_object->_range = physical_alloc(size);

    list_pushback(_memory_objects, memory_object);

    return memory_object;
}

void memory_object_destroy(MemoryObject *memory_object)
{
    list_remove(_memory_objects, memory_object);

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

    list_foreach(MemoryObject, memory_object, _memory_objects)
    {
        if (memory_object->id == id)
        {
            memory_object_ref(memory_object);
            return memory_object;
        }
    }

    return nullptr;
}
