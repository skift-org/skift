#pragma once

#include "kernel/tasking.h"

size_t platform_page_size(void);

/* --- Processor's FPU ------------------------------------------------------ */

void platform_fpu_enable(void);

void platform_fpu_save_context(Task *task);

void platform_fpu_load_context(Task *task);

/* --- Processor ------------------------------------------------------------ */

void platform_initialize(void);

void platform_save_context(Task *task);

void platform_load_context(Task *task);
