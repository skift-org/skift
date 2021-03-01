#pragma once

#include "kernel/memory/MemoryObject.h"
#include "kernel/tasking/Task.h"

struct MemoryMapping
{
    MemoryObject *object;

    uintptr_t address;
    size_t size;

    MemoryRange range() { return {address, size}; }
};

MemoryMapping *task_memory_mapping_create(Task *task, MemoryObject *memory_object);

void task_memory_mapping_destroy(Task *task, MemoryMapping *memory_mapping);

MemoryMapping *task_memory_mapping_by_address(Task *task, uintptr_t address);

Result task_memory_alloc(Task *task, size_t size, uintptr_t *out_address);

Result task_memory_map(Task *task, uintptr_t address, size_t size, MemoryFlags flags);

Result task_memory_free(Task *task, uintptr_t address);

Result task_memory_include(Task *task, int handle, uintptr_t *out_address, size_t *out_size);

Result task_memory_get_handle(Task *task, uintptr_t address, int *out_handle);

void *task_switch_address_space(Task *task, void *address_space);

size_t task_memory_usage(Task *task);
