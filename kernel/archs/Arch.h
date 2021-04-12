#pragma once

#include <abi/Memory.h>
#include <libsystem/Result.h>
#include <skift/Time.h>

#include "kernel/memory/MemoryRange.h"

struct Task;

void arch_disable_interrupts();

void arch_enable_interrupts();

void arch_halt();

void arch_yield();

void arch_save_context(Task *task);

void arch_load_context(Task *task);

void arch_task_go(Task *task);

size_t arch_debug_write(const void *buffer, size_t size);

TimeStamp arch_get_time();

NO_RETURN void arch_reboot();

NO_RETURN void arch_shutdown();

void arch_panic_dump();

void arch_dump_stack_frame(void *stackframe);

void arch_backtrace();

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
