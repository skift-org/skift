#pragma once

#include <skift/runtime.h>
#include <skift/syscalls.h>

DECL_SYSCALL0(process_self);
DECL_SYSCALL2(process_exec, const char * path, const char ** argv);
DECL_SYSCALL1(process_exit, int code);
DECL_SYSCALL1(process_cancel, int pid);
DECL_SYSCALL2(process_map, unsigned int addr, unsigned int count);
DECL_SYSCALL2(process_unmap, unsigned int addr, unsigned int count);
DECL_SYSCALL1(process_alloc, unsigned int count);
DECL_SYSCALL2(process_free, unsigned int addr, unsigned int count);