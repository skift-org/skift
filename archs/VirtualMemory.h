#pragma once

#include <abi/Memory.h>

#include <libsystem/Result.h>

#include "kernel/memory/MemoryRange.h"

void *arch_kernel_address_space();

void arch_virtual_initialize();

void arch_virtual_memory_enable();

bool arch_virtual_present(void *address_space, uintptr_t virtual_address);

uintptr_t arch_virtual_to_physical(void *address_space, uintptr_t virtual_address);

Result arch_virtual_map(void *address_space, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags);

MemoryRange arch_virtual_alloc(void *address_space, MemoryRange physical_range, MemoryFlags flags);

void arch_virtual_free(void *address_space, MemoryRange virtual_range);

void *arch_address_space_create();

void arch_address_space_destroy(void *address_space);

void arch_address_space_switch(void *address_space);

