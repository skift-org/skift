#pragma once

#include <abi/Process.h>

#include <libsystem/Common.h>
#include <libsystem/Result.h>

int process_this(void);

Result process_run(const char *command, int *pid);

void __no_return process_exit(int code);

Result process_cancel(int pid);

Result process_map(uintptr_t address, size_t size);

Result process_alloc(size_t size, uintptr_t *out_address);

Result process_free(uintptr_t address, size_t size);

Result process_get_directory(char *buffer, size_t size);

Result process_set_directory(const char *directory);

int process_sleep(int time);

int process_wait(int pid, int *exit_value);
