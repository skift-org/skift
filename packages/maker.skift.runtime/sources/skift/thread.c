/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/thread.h>

DEFN_SYSCALL0(sk_thread_self, SYS_THREAD_SELF);
DEFN_SYSCALL2(sk_thread_create, SYS_THREAD_CREATE, int, void *);

DEFN_SYSCALL1(sk_thread_exit, SYS_THREAD_EXIT, void *);
DEFN_SYSCALL1(sk_thread_cancel, SYS_THREAD_CANCEL, int);

DEFN_SYSCALL1(sk_thread_sleep, SYS_THREAD_SLEEP, int);
DEFN_SYSCALL1(sk_thread_wakeup, SYS_THREAD_WAKEUP, int);

DEFN_SYSCALL1(sk_thread_wait_thread,  SYS_THREAD_WAIT_THREAD,  int);
DEFN_SYSCALL1(sk_thread_wait_process, SYS_THREAD_WAIT_PROCESS, int);
DEFN_SYSCALL1(sk_thread_wait_message, SYS_THREAD_WAIT_MESSAGE, message_t *);