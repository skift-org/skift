#include <libsystem/core/CString.h>
#include <libsystem/thread/Atomic.h>

#include "arch/VirtualMemory.h"

#include "kernel/tasking/Task-Memory.h"

MemoryMapping *task_memory_mapping_create(Task *task, MemoryObject *memory_object)
{
    AtomicHolder holder;

    MemoryMapping *memory_mapping = __create(MemoryMapping);

    memory_mapping->object = memory_object_ref(memory_object);
    memory_mapping->address = arch_virtual_alloc(task->address_space, memory_object->range(), MEMORY_USER).base();
    memory_mapping->size = memory_object->range().size();

    list_pushback(task->memory_mapping, memory_mapping);

    return memory_mapping;
}

MemoryMapping *task_memory_mapping_create_at(Task *task, MemoryObject *memory_object, uintptr_t address)
{
    AtomicHolder holder;

    MemoryMapping *memory_mapping = __create(MemoryMapping);

    memory_mapping->object = memory_object_ref(memory_object);
    memory_mapping->address = arch_virtual_map(task->address_space, memory_object->range(), address, MEMORY_USER);
    memory_mapping->size = memory_object->range().size();

    list_pushback(task->memory_mapping, memory_mapping);

    return memory_mapping;
}

void task_memory_mapping_destroy(Task *task, MemoryMapping *memory_mapping)
{
    AtomicHolder holder;

    arch_virtual_free(task->address_space, (MemoryRange){memory_mapping->address, memory_mapping->size});
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

    return nullptr;
}

bool task_memory_mapping_colides(Task *task, uintptr_t address, size_t size)
{
    list_foreach(MemoryMapping, memory_mapping, task->memory_mapping)
    {
        if (address < memory_mapping->address + memory_mapping->size &&
            address + size > memory_mapping->address)
        {
            return true;
        }
    }

    return false;
}

/* --- User facing API ------------------------------------------------------ */

Result task_memory_alloc(Task *task, size_t size, uintptr_t *out_address)
{
    MemoryObject *memory_object = memory_object_create(size);

    MemoryMapping *memory_mapping = task_memory_mapping_create(task, memory_object);

    memory_object_deref(memory_object);

    *out_address = memory_mapping->address;

    return SUCCESS;
}

Result task_memory_map(Task *task, uintptr_t address, size_t size, MemoryFlags flags)
{
    if (task_memory_mapping_colides(task, address, size))
    {
        return ERR_BAD_ADDRESS;
    }

    MemoryObject *memory_object = memory_object_create(size);

    task_memory_mapping_create_at(task, memory_object, address);

    memory_object_deref(memory_object);

    if (flags & MEMORY_CLEAR)
    {
        memset((void *)address, 0, size);
    }

    return SUCCESS;
}

Result task_memory_free(Task *task, uintptr_t address)
{
    MemoryMapping *memory_mapping = task_memory_mapping_by_address(task, address);

    if (!memory_mapping)
    {
        return ERR_BAD_ADDRESS;
    }

    task_memory_mapping_destroy(task, memory_mapping);

    return SUCCESS;
}

Result task_memory_include(Task *task, int handle, uintptr_t *out_address, size_t *out_size)
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

Result task_memory_get_handle(Task *task, uintptr_t address, int *out_handle)
{
    MemoryMapping *memory_mapping = task_memory_mapping_by_address(task, address);

    if (!memory_mapping)
    {
        return ERR_BAD_ADDRESS;
    }

    *out_handle = memory_mapping->object->id;
    return SUCCESS;
}

void *task_switch_address_space(Task *task, void *address_space)
{
    void *old_address_space = task->address_space;

    task->address_space = address_space;

    arch_address_space_switch(address_space);

    return old_address_space;
}

size_t task_memory_usage(Task *task)
{
    size_t total = 0;

    list_foreach(MemoryMapping, memory_mapping, task->memory_mapping)
    {
        total += memory_mapping->size;
    }

    return total;
}
