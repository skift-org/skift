#pragma once

#include <skift/runtime.h>
#include <skift/syscalls.h>

DECL_SYSCALL1(io_print, const char * msg);
DECL_SYSCALL2(io_read, char * buffer, int size);
