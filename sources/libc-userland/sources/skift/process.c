/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/process.h>
#include <skift/syscall.h>

DEFN_SYSCALL0(sk_process_self, SYS_PROCESS_SELF);
DEFN_SYSCALL2(sk_process_exec, SYS_PROCESS_EXEC, const char *, const char **);

DEFN_SYSCALL1(sk_process_exit, SYS_PROCESS_EXIT, int);
DEFN_SYSCALL1(sk_process_cancel, SYS_PROCESS_CANCEL, int);

DEFN_SYSCALL2(sk_process_map, SYS_PROCESS_MAP, unsigned int, unsigned int);
DEFN_SYSCALL2(sk_process_unmap, SYS_PROCESS_UNMAP, unsigned int, unsigned int);

DEFN_SYSCALL1(sk_process_alloc, SYS_PROCESS_ALLOC, unsigned int);
DEFN_SYSCALL2(sk_process_free,  SYS_PROCESS_FREE, unsigned int, unsigned int);