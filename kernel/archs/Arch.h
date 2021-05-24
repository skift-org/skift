#pragma once

#include <abi/Memory.h>
#include <libsystem/Result.h>
#include <skift/Time.h>

#include "system/memory/MemoryRange.h"

struct Task;

namespace Arch
{

void disable_interrupts();

void enable_interrupts();

void halt();

void yield();

void save_context(Task *task);

void load_context(Task *task);

void task_go(Task *task);

size_t debug_write(const void *buffer, size_t size);

TimeStamp get_time();

NO_RETURN void reboot();

NO_RETURN void shutdown();

void dump_stack_frame(void *stackframe);

void backtrace();

void *kernel_address_space();

void virtual_initialize();

void virtual_memory_enable();

bool virtual_present(void *address_space, uintptr_t virtual_address);

uintptr_t virtual_to_physical(void *address_space, uintptr_t virtual_address);

Result virtual_map(void *address_space, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags);

MemoryRange virtual_alloc(void *address_space, MemoryRange physical_range, MemoryFlags flags);

void virtual_free(void *address_space, MemoryRange virtual_range);

void *address_space_create();

void address_space_destroy(void *address_space);

void address_space_switch(void *address_space);

} // namespace Arch
