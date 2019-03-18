#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/thread.h"

void sheduler_yield();

thread_t* sheduler_running_thread();
int sheduler_running_thread_id();

void sheduler_setup(thread_t* main_kernel_thread, PROCESS kernel_process);