#pragma once

#include <__libc__.h>

__BEGIN_HEADER

#define SIG_ERR ((sighandler_t)(void *)(-1))
#define SIG_DFL ((sighandler_t)(void *)(-2))
#define SIG_IGN ((sighandler_t)(void *)(-3))

#define SIGABRT 1
#define SIGFPE 2
#define SIGILL 3
#define SIGINT 4
#define SIGSEGV 5
#define SIGTERM 6
#define SIGPROF 7
#define SIGIO 9
#define SIGPWR 10
#define SIGRTMIN 11
#define SIGRTMAX 12
#define SIGALRM 8
#define SIGBUS 9
#define SIGCHLD 10
#define SIGCONT 11
#define SIGHUP 12
#define SIGKILL 13
#define SIGPIPE 14
#define SIGQUIT 15
#define SIGSTOP 16
#define SIGTSTP 17
#define SIGTTIN 18
#define SIGTTOU 19
#define SIGUSR1 20
#define SIGUSR2 21
#define SIGSYS 22
#define SIGTRAP 23
#define SIGURG 24
#define SIGVTALRM 25
#define SIGXCPU 26
#define SIGXFSZ 27
#define SIGWINCH 28
#define SIGUNUSED SIGSYS

typedef int sig_atomic_t;

typedef void (*sighandler_t)(int);
sighandler_t signal(int sig, sighandler_t handler);

int raise(int sig);

__END_HEADER
