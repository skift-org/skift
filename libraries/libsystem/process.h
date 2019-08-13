#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

int process_this(void);

int process_exec(const char *file_name, const char **argv);

// TODO: void process_spawn();

int process_exit(int code);

int process_cancel(int pid);

int process_map(uint addr, uint count);

int process_unmap(uint addr, uint count);

uint process_alloc(uint count);

int process_free(uint addr, uint count);

int process_get_cwd(char* buffer, uint size);

int process_set_cwd(const char* cwd);

int process_sleep(int time);

int process_wakeup(int pid);

int process_wait(int pid, int* exit_value);