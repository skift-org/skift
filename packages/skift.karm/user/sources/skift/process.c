/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/process.h>

DEFN_SYSCALL0(process_self, SYS_PROCESS_SELF);
DEFN_SYSCALL2(process_exec, SYS_PROCESS_EXEC, const char *, const char **);

DEFN_SYSCALL1(process_exit, SYS_PROCESS_EXIT, int);
DEFN_SYSCALL1(process_cancel, SYS_PROCESS_CANCEL, int);

DEFN_SYSCALL2(process_map, SYS_PROCESS_MAP, unsigned int, unsigned int);
DEFN_SYSCALL2(process_unmap, SYS_PROCESS_UNMAP, unsigned int, unsigned int);

DEFN_SYSCALL1(process_alloc, SYS_PROCESS_ALLOC, unsigned int);
DEFN_SYSCALL2(process_free,  SYS_PROCESS_FREE, unsigned int, unsigned int);