#pragma once

#include <libsystem/Common.h>
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
    __ENTRY(SYS_SYSTEM_SHUTDOWN)       \
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

static inline Result __syscall(Syscall syscall, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5)
{
    Result __ret = ERR_FUNCTION_NOT_IMPLEMENTED;

#if defined(__x86_64__)
    // TODO: x86_64 syscall.
    __unused(syscall);
    __unused(p1);
    __unused(p2);
    __unused(p3);
    __unused(p4);
    __unused(p5);

#elif defined(__i386__)
    __asm__ __volatile__("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx"
                         : "=a"(__ret)
                         : "0"(syscall), "r"(p1), "c"(p2), "d"(p3), "S"(p4), "D"(p5)
                         : "memory");
#endif

    return __ret;
}

static inline Result __syscall(Syscall syscall, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4)
{
    return __syscall(syscall, p1, p2, p3, p4, 0);
}

static inline Result __syscall(Syscall syscall, uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
    return __syscall(syscall, p1, p2, p3, 0, 0);
}

static inline Result __syscall(Syscall syscall, uintptr_t p1, uintptr_t p2)
{
    return __syscall(syscall, p1, p2, 0, 0, 0);
}

static inline Result __syscall(Syscall syscall, uintptr_t p1)
{
    return __syscall(syscall, p1, 0, 0, 0, 0);
}

static inline Result __syscall(Syscall syscall)
{
    return __syscall(syscall, 0, 0, 0, 0, 0);
}
