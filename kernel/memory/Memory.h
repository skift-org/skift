#pragma once

#include <abi/Memory.h>
#include <libsystem/Result.h>

#include "kernel/handover/Handover.h"
#include "kernel/memory/MemoryRange.h"

void memory_initialize(Handover *handover);

void memory_dump();

size_t memory_get_used();

size_t memory_get_total();

Result memory_map(void *address_space, MemoryRange range, MemoryFlags flags);

Result memory_map_identity(void *address_space, MemoryRange range, MemoryFlags flags);

Result memory_alloc(void *address_space, size_t size, MemoryFlags flags, uintptr_t *out_address);

Result memory_alloc_identity(void *address_space, MemoryFlags flags, uintptr_t *out_address);

Result memory_free(void *address_space, MemoryRange range);
