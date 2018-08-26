#include "syscalls.h"

DEFN_SYSCALL1(exit, 0, int);
DEFN_SYSCALL1(print, 1, const char *);