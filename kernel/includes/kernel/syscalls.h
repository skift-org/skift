#pragma once
#include "types.h"

typedef int (*syscalls_t)(int, int, int, int, int);

#define SYSCALL_COUNT 9

#define SYS_OPEN 0
#define SYS_CLOSE 1
#define SYS_READ 2
#define SYS_WRITE 3
#define SYS_IOCTL 4

#define SYS_EXEC 5
#define SYS_EXIT 6

#define SYS_BRK 7
#define SYS_SBRK 8
