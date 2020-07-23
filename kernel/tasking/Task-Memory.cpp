#include "kernel/tasking/Task-Memory.h"
#include "kernel/memory/Virtual.h"

MemoryMapping *task_memory_mapping_create(Task *task, MemoryObject *memory_object)
{
    MemoryMapping *memory_mapping = __create(MemoryMapping);

    memory_mapping->object = memory_object_ref(memory_object);
    memory_mapping->address = virtual_alloc(task->pdir, (MemoryRange){memory_object->address, memory_object->size}, MEMORY_USER).base;
    memory_mapping->size = memory_object->size;

    list_pushback(task->memory_mapping, memory_mapping);

    return memory_mapping;
}

void task_memory_mapping_destroy(Task *task, MemoryMapping *memory_mapping)
{
    virtual_free(task->pdir, (MemoryRange){memory_mapping->address, memory_mapping->size});
    memory_object_deref(memory_mapping->object);

    list_remove(task->memory_mapping, memory_mapping);
    free(memory_mapping);
}

MemoryMapping *task_memory_mapping_by_address(Task *task, uintptr_t address)
{
    list_foreach(MemoryMapping, memory_mapping, task->memory_mapping)
    {
        if (memory_mapping->address == address)
        {
            return memory_mapping;
        }
    }

    return NULL;
}

/* --- User facing API ------------------------------------------------------ */

Result task_shared_memory_alloc(Task *task, size_t size, uintptr_t *out_address)
{
    MemoryObject *memory_object = memory_object_create(size);

    MemoryMapping *memory_mapping = task_memory_mapping_create(task, memory_object);

    memory_object_deref(memory_object);

    *out_address = memory_mapping->address;

    return SUCCESS;
}

Result task_shared_memory_free(Task *task, uintptr_t address)
{
    MemoryMapping *memory_mapping = task_memory_mapping_by_address(task, address);

    if (!memory_mapping)
    {
        return ERR_BAD_ADDRESS;
    }

    task_memory_mapping_destroy(task, memory_mapping);

    return SUCCESS;
}

Result task_shared_memory_include(Task *task, int handle, uintptr_t *out_address, size_t *out_size)
{
    MemoryObject *memory_object = memory_object_by_id(handle);

    if (!memory_object)
    {
        return ERR_BAD_ADDRESS;
    }

    MemoryMapping *memory_mapping = task_memory_mapping_create(task, memory_object);

    memory_object_deref(memory_object);

    *out_address = memory_mapping->address;
    *out_size = memory_mapping->size;

    return SUCCESS;
}

Result task_shared_memory_get_handle(Task *task, uintptr_t address, int *out_handle)
{
    MemoryMapping *memory_mapping = task_memory_mapping_by_address(task, address);

    if (!memory_mapping)
    {
        return ERR_BAD_ADDRESS;
    }

    *out_handle = memory_mapping->object->id;
    return SUCCESS;
}

PageDirectory *task_switch_pdir(Task *task, PageDirectory *pdir)
{
    PageDirectory *oldpdir = task->pdir;

    task->pdir = pdir;

    memory_pdir_switch(pdir);

    return oldpdir;
}

Result task_memory_map(Task *task, MemoryRange range)
{
    return memory_map(task->pdir, range, MEMORY_USER | MEMORY_CLEAR);
}

Result task_memory_alloc(Task *task, size_t size, uintptr_t *out_address)
{
    return memory_alloc(task->pdir, size, MEMORY_USER | MEMORY_CLEAR, out_address);
}

Result task_memory_free(Task *task, MemoryRange range)
{
    return memory_free(task->pdir, range);
}