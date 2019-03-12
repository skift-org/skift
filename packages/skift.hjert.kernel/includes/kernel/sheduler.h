#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/thread.h"

thread_t* sheduler_running_thread();

void sheduler_setup(thread_t* main_kernel_thread, PROCESS kernel_process);