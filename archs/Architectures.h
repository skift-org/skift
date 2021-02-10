#pragma once

#include <skift/Time.h>

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

__no_return void arch_reboot();

__no_return void arch_shutdown();

void arch_panic_dump();

void arch_dump_stack_frame(void *stackframe);

void arch_backtrace();
