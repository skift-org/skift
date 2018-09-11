#include <skift/thread.h>
#include <skift/syscall.h>

DEFN_SYSCALL0(sk_thread_self, SYS_THREAD_SELF);
DEFN_SYSCALL3(sk_thread_create, SYS_THREAD_CREATE, int, void *, int);
DEFN_SYSCALL1(sk_thread_exit, SYS_THREAD_EXIT, void *);
DEFN_SYSCALL1(sk_thread_cancel, SYS_THREAD_CANCEL, int);
DEFN_SYSCALL1(sk_thread_sleep, SYS_THREAD_SLEEP, int);
DEFN_SYSCALL1(sk_thread_wakeup, SYS_THREAD_WAKEUP, int);
DEFN_SYSCALL1(sk_thread_wait, SYS_THREAD_WAIT, int);
DEFN_SYSCALL1(sk_thread_waitproc, SYS_THREAD_WAITPROC, int);