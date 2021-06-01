#pragma once

#include <abi/Process.h>
#include <abi/Task.h>

#include <abi/Result.h>
#include <libutils/String.h>

int process_this();

const char *process_name();

HjResult process_run(const char *command, int *pid, TaskFlags flags);

void NO_RETURN process_abort();

void NO_RETURN process_exit(int code);

HjResult process_cancel(int pid);

HjResult process_get_directory(char *buffer, size_t size);

HjResult process_set_directory(const char *directory);

String process_resolve(String path);

HjResult process_sleep(int time);

HjResult process_wait(int pid, int *exit_value);
