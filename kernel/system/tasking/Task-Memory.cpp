#include <string.h>

#include "archs/Arch.h"

#include "system/interrupts/Interupts.h"
#include "system/tasking/Task-Memory.h"

static bool will_i_be_kill_if_i_allocate_that(Task *task, size_t size)
{
    auto usage = task_memory_usage(task);

    if (usage + size > memory_get_total() / 2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void kill_me_if_too_greedy(Task *task, size_t size)
{
    if (will_i_be_kill_if_i_allocate_that(task, size))
    {
        task->handles().write(2, "(ulimit reached)\n", 17);
        task->cancel(PROCESS_FAILURE);
    }
}

MemoryMapping *task_memory_mapping_create(Task *task, MemoryObject *memory_object)
{
    InterruptsRetainer retainer;

    auto memory_mapping = CREATE(MemoryMapping);

    memory_mapping->object = memory_object_ref(memory_object);
    memory_mapping->address = Arch::virtual_alloc(task->address_space, memory_object->range(), MEMORY_USER).base();
    memory_mapping->size = memory_object->range().size();

    task->memory_mapping->push_back(memory_mapping);

    return memory_mapping;
}

MemoryMapping *task_memory_mapping_create_at(Task *task, MemoryObject *memory_object, uintptr_t address)
{
    InterruptsRetainer retainer;

    auto memory_mapping = CREATE(MemoryMapping);

    memory_mapping->object = memory_object_ref(memory_object);
    memory_mapping->address = address;
    memory_mapping->size = memory_object->range().size();

    assert(SUCCESS == Arch::virtual_map(task->address_space, memory_object->range(), address, MEMORY_USER));

    task->memory_mapping->push_back(memory_mapping);

    return memory_mapping;
}

void task_memory_mapping_destroy(Task *task, MemoryMapping *memory_mapping)
{
    InterruptsRetainer retainer;

    Arch::virtual_free(task->address_space, (MemoryRange){memory_mapping->address, memory_mapping->size});
    memory_object_deref(memory_mapping->object);

    task->memory_mapping->remove(memory_mapping);
    free(memory_mapping);
}

MemoryMapping *task_memory_mapping_by_address(Task *task, uintptr_t address)
{
    for (auto *memory_mapping : *task->memory_mapping)
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
    for (auto *memory_mapping : *task->memory_mapping)
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

HjResult task_memory_alloc(Task *task, size_t size, uintptr_t *out_address)
{
    kill_me_if_too_greedy(task, size);

    auto memory_object = memory_object_create(size);

    auto memory_mapping = task_memory_mapping_create(task, memory_object);

    memory_object_deref(memory_object);

    *out_address = memory_mapping->address;

    return SUCCESS;
}

HjResult task_memory_map(Task *task, uintptr_t address, size_t size, MemoryFlags flags)
{
    kill_me_if_too_greedy(task, size);

    if (task_memory_mapping_colides(task, address, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto memory_object = memory_object_create(size);

    task_memory_mapping_create_at(task, memory_object, address);

    memory_object_deref(memory_object);

    if (flags & MEMORY_CLEAR)
    {
        memset((void *)address, 0, size);
    }

    return SUCCESS;
}

HjResult task_memory_free(Task *task, uintptr_t address)
{
    auto memory_mapping = task_memory_mapping_by_address(task, address);

    if (!memory_mapping)
    {
        return ERR_BAD_ADDRESS;
    }

    task_memory_mapping_destroy(task, memory_mapping);

    return SUCCESS;
}

HjResult task_memory_include(Task *task, int handle, uintptr_t *out_address, size_t *out_size)
{
    auto memory_object = memory_object_by_id(handle);

    if (!memory_object)
    {
        return ERR_BAD_ADDRESS;
    }

    if (will_i_be_kill_if_i_allocate_that(task, memory_object->range().size()))
    {
        memory_object_deref(memory_object);
        kill_me_if_too_greedy(task, memory_object->range().size());
    }

    auto memory_mapping = task_memory_mapping_create(task, memory_object);

    memory_object_deref(memory_object);

    *out_address = memory_mapping->address;
    *out_size = memory_mapping->size;

    return SUCCESS;
}

HjResult task_memory_get_handle(Task *task, uintptr_t address, int *out_handle)
{
    auto memory_mapping = task_memory_mapping_by_address(task, address);

    if (!memory_mapping)
    {
        return ERR_BAD_ADDRESS;
    }

    *out_handle = memory_mapping->object->id;
    return SUCCESS;
}

Arch::AddressSpace *task_switch_address_space(Task *task, Arch::AddressSpace *address_space)
{
    auto *old_address_space = task->address_space;

    task->address_space = address_space;

    Arch::address_space_switch(address_space);

    return old_address_space;
}

size_t task_memory_usage(Task *task)
{
    size_t total = 0;

    for (auto *memory_mapping : *task->memory_mapping)
    {
        total += memory_mapping->size;
    }

    return total;
}
