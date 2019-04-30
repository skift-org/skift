#pragma once

#include <skift/runtime.h>
#include <skift/syscalls.h>

DECL_SYSCALL0(sk_process_self);
DECL_SYSCALL2(sk_process_exec, const char * path, const char ** argv);
DECL_SYSCALL1(sk_process_exit, int code);
DECL_SYSCALL1(sk_process_cancel, int pid);
DECL_SYSCALL2(sk_process_map, unsigned int addr, unsigned int count);
DECL_SYSCALL2(sk_process_unmap, unsigned int addr, unsigned int count);
DECL_SYSCALL1(sk_process_alloc, unsigned int count);
DECL_SYSCALL2(sk_process_free, unsigned int addr, unsigned int count);