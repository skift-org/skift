#pragma once

#include <abi/Memory.h>
#include <abi/Result.h>

#include "archs/Arch.h"
#include "system/handover/Handover.h"
#include "system/memory/MemoryRange.h"

void memory_initialize(Handover *handover);

void memory_dump();

size_t memory_get_used();

size_t memory_get_total();

HjResult memory_map(Arch::AddressSpace *address_space, MemoryRange range, MemoryFlags flags);

HjResult memory_map_identity(Arch::AddressSpace *address_space, MemoryRange range, MemoryFlags flags);

HjResult memory_alloc(Arch::AddressSpace *address_space, size_t size, MemoryFlags flags, uintptr_t *out_address);

HjResult memory_alloc_identity(Arch::AddressSpace *address_space, MemoryFlags flags, uintptr_t *out_address);

HjResult memory_free(Arch::AddressSpace *address_space, MemoryRange range);
