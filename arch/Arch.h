#pragma once

#include "kernel/tasking/Task.h"

void arch_initialize(void);

void arch_disable_interupts(void);

void arch_enable_interupts(void);

void arch_halt(void);

void arch_yield(void);

void arch_save_context(Task *task);

void arch_load_context(Task *task);

size_t arch_debug_write(const void *buffer, size_t size);

TimeStamp arch_get_time(void);
