#pragma once

#include <libsystem/Result.h>

#define SYSCALL_LIST(__ENTRY)          \
    __ENTRY(SYS_PROCESS_THIS)          \
    __ENTRY(SYS_PROCESS_LAUNCH)        \
    __ENTRY(SYS_PROCESS_EXIT)          \
    __ENTRY(SYS_PROCESS_CANCEL)        \
    __ENTRY(SYS_PROCESS_SLEEP)         \
    __ENTRY(SYS_PROCESS_WAIT)          \
    __ENTRY(SYS_PROCESS_GET_DIRECTORY) \
    __ENTRY(SYS_PROCESS_SET_DIRECTORY) \
                                       \
    __ENTRY(SYS_MEMORY_ALLOC)          \
    __ENTRY(SYS_MEMORY_FREE)           \
    __ENTRY(SYS_MEMORY_INCLUDE)        \
    __ENTRY(SYS_MEMORY_GET_HANDLE)     \
                                       \
    __ENTRY(SYS_FILESYSTEM_LINK)       \
    __ENTRY(SYS_FILESYSTEM_UNLINK)     \
    __ENTRY(SYS_FILESYSTEM_RENAME)     \
    __ENTRY(SYS_FILESYSTEM_MKPIPE)     \
    __ENTRY(SYS_FILESYSTEM_MKDIR)      \
                                       \
    __ENTRY(SYS_SYSTEM_GET_INFO)       \
    __ENTRY(SYS_SYSTEM_GET_STATUS)     \
    __ENTRY(SYS_SYSTEM_GET_TIME)       \
    __ENTRY(SYS_SYSTEM_GET_TICKS)      \
    __ENTRY(SYS_SYSTEM_REBOOT)         \
                                       \
    __ENTRY(SYS_HANDLE_OPEN)           \
    __ENTRY(SYS_HANDLE_CLOSE)          \
    __ENTRY(SYS_HANDLE_SELECT)         \
    __ENTRY(SYS_HANDLE_READ)           \
    __ENTRY(SYS_HANDLE_WRITE)          \
    __ENTRY(SYS_HANDLE_CALL)           \
    __ENTRY(SYS_HANDLE_SEEK)           \
    __ENTRY(SYS_HANDLE_TELL)           \
    __ENTRY(SYS_HANDLE_STAT)           \
    __ENTRY(SYS_HANDLE_CONNECT)        \
    __ENTRY(SYS_HANDLE_ACCEPT)         \
                                       \
    __ENTRY(SYS_CREATE_PIPE)           \
    __ENTRY(SYS_CREATE_TERM)

#define SYSCALL_ENUM_ENTRY(__entry) __entry,

enum Syscall
{
    SYSCALL_LIST(SYSCALL_ENUM_ENTRY) __SYSCALL_COUNT
};

static inline Result __syscall(Syscall syscall, int p1, int p2, int p3, int p4, int p5)
{
    Result __ret;
    __asm__ __volatile__("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx"
                         : "=a"(__ret)
                         : "0"(syscall), "r"((int)(p1)), "c"((int)(p2)), "d"((int)(p3)), "S"((int)(p4)), "D"((int)(p5))
                         : "memory");
    return __ret;
}

static inline Result __syscall(Syscall syscall, int p1, int p2, int p3, int p4)
{
    return __syscall(syscall, p1, p2, p3, p4, 0);
}

static inline Result __syscall(Syscall syscall, int p1, int p2, int p3)
{
    return __syscall(syscall, p1, p2, p3, 0, 0);
}

static inline Result __syscall(Syscall syscall, int p1, int p2)
{
    return __syscall(syscall, p1, p2, 0, 0, 0);
}

static inline Result __syscall(Syscall syscall, int p1)
{
    return __syscall(syscall, p1, 0, 0, 0, 0);
}

static inline Result __syscall(Syscall syscall)
{
    return __syscall(syscall, 0, 0, 0, 0, 0);
}
