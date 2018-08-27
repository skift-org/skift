#include "syscalls.h"

DEFN_SYSCALL1(exit, 0, int);

DEFN_SYSCALL1(print, 1, const char *);

DEFN_SYSCALL2(map, 2, void*, int);

DEFN_SYSCALL2(unmap, 3, void*, int);