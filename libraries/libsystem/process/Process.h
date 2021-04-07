#pragma once

#include <abi/Process.h>
#include <abi/Task.h>

#include <libsystem/Result.h>
#include <libutils/String.h>

int process_this();

const char *process_name();

Result process_run(const char *command, int *pid, TaskFlags flags);

void NO_RETURN process_abort();

void NO_RETURN process_exit(int code);

Result process_cancel(int pid);

Result process_get_directory(char *buffer, size_t size);

Result process_set_directory(const char *directory);

String process_resolve(String path);

Result process_sleep(int time);

Result process_wait(int pid, int *exit_value);
