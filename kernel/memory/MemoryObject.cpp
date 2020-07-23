#include <libsystem/thread/Lock.h>
#include <libsystem/utils/List.h>

#include "kernel/memory/Memory.h"
#include "kernel/memory/MemoryObject.h"
#include "kernel/memory/Physical.h"

static int _memory_object_id = 0;
static List *_memory_objects;
static Lock _memory_objects_lock;

void memory_object_initialize(void)
{
    lock_init(_memory_objects_lock);
    _memory_objects = list_create();
}

MemoryObject *memory_object_create(size_t size)
{
    size = PAGE_ALIGN_UP(size);

    MemoryObject *memory_object = __create(MemoryObject);

    memory_object->id = _memory_object_id++;
    memory_object->refcount = 1;
    memory_object->address = physical_alloc(size / PAGE_SIZE);
    memory_object->size = size;

    lock_acquire(_memory_objects_lock);
    list_pushback(_memory_objects, memory_object);
    lock_release(_memory_objects_lock);

    return memory_object;
}

void memory_object_destroy(MemoryObject *memory_object)
{
    list_remove(_memory_objects, memory_object);

    physical_free(memory_object->address, PAGE_ALIGN_UP(memory_object->size) / PAGE_SIZE);
    free(memory_object);
}

MemoryObject *memory_object_ref(MemoryObject *memory_object)
{
    __atomic_add_fetch(&memory_object->refcount, 1, __ATOMIC_SEQ_CST);

    return memory_object;
}

void memory_object_deref(MemoryObject *memory_object)
{
    lock_acquire(_memory_objects_lock);

    if (__atomic_sub_fetch(&memory_object->refcount, 1, __ATOMIC_SEQ_CST) == 0)
    {
        memory_object_destroy(memory_object);
    }

    lock_release(_memory_objects_lock);
}

MemoryObject *memory_object_by_id(int id)
{
    lock_acquire(_memory_objects_lock);

    list_foreach(MemoryObject, memory_object, _memory_objects)
    {
        if (memory_object->id == id)
        {
            memory_object_ref(memory_object);

            lock_release(_memory_objects_lock);

            return memory_object;
        }
    }

    lock_release(_memory_objects_lock);

    return NULL;
}
