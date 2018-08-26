#pragma once
#include "types.h"
#include "cpu/cpu.h"

int sys_exit(int code);
int sys_print(const char * string);

void syscall_dispatcher(context_t * context);