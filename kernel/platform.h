#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "tasking.h"
#include "processor.h"

/* --- Processor's FPU ------------------------------------------------------ */

void platform_fpu_enable(void);

void platform_fpu_save_context(task_t* t);

void platform_fpu_load_context(task_t* t);

/* --- Processor ------------------------------------------------------------ */

void platform_setup(void);

void platform_save_context(task_t* t);

void platform_load_context(task_t* t);