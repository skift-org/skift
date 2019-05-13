#pragma once

#include <skift/runtime.h>
#include <skift/syscalls.h>

DECL_SYSCALL0(thread_self);
DECL_SYSCALL2(thread_create, int entry, void *arg);
DECL_SYSCALL1(thread_exit, void *exitval);
DECL_SYSCALL2(thread_cancel, int thread, void *exitval);
DECL_SYSCALL1(thread_sleep, int time);
DECL_SYSCALL1(thread_wakeup, int thread);
DECL_SYSCALL2(thread_wait_thread, int thread, int* exitvalue);
DECL_SYSCALL2(thread_wait_process, int process, int* exitvalue);