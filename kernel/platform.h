#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/tasking.h"

size_t platform_page_size(void);

/* --- Processor's FPU ------------------------------------------------------ */

void platform_fpu_enable(void);

void platform_fpu_save_context(Task *task);

void platform_fpu_load_context(Task *task);

/* --- Processor ------------------------------------------------------------ */

void platform_setup(void);

void platform_save_context(Task *task);

void platform_load_context(Task *task);
