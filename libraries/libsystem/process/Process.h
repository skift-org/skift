#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Process.h>

#include <libsystem/Result.h>
#include <libsystem/runtime.h>

int process_this(void);

Result process_run(const char *command, int *pid);

void __attribute__((noreturn)) process_exit(int code);

int process_cancel(int pid);

int process_map(uintptr_t addr, size_t count);

int process_unmap(uintptr_t addr, size_t count);

uint process_alloc(size_t count);

int process_free(size_t addr, size_t count);

Result process_get_cwd(char *buffer, size_t size);

Result process_set_cwd(const char *cwd);

int process_sleep(int time);

int process_wakeup(int pid);

int process_wait(int pid, int *exit_value);