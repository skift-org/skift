#pragma once

#include <abi/Syscalls.h>

#include <libutils/Prelude.h>

uintptr_t task_do_syscall(Syscall syscall, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4);
