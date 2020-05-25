#pragma once

#include <abi/Syscalls.h>

#include <libsystem/Common.h>

int task_do_syscall(Syscall syscall, int arg0, int arg1, int arg2, int arg3, int arg4);
