#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

typedef enum
{
    SYS_DEBUG = 0,

    SYS_PROCESS_SELF,
    SYS_PROCESS_EXEC,
    SYS_PROCESS_SPAWN,
    SYS_PROCESS_EXIT,
    SYS_PROCESS_CANCEL,
    SYS_PROCESS_SLEEP,
    SYS_PROCESS_WAKEUP,
    SYS_PROCESS_WAIT,
    SYS_PROCESS_GET_CWD,
    SYS_PROCESS_SET_CWD,
    SYS_PROCESS_MAP,
    SYS_PROCESS_UNMAP,
    SYS_PROCESS_ALLOC,
    SYS_PROCESS_FREE,
    
    SYS_SHARED_MEMORY_ALLOC,
    SYS_SHARED_MEMORY_ACQUIRE,
    SYS_SHARED_MEMORY_RELEASE,

    SYS_MESSAGING_SEND,
    SYS_MESSAGING_BROADCAST,
    SYS_MESSAGING_REQUEST,
    SYS_MESSAGING_RECEIVE,
    SYS_MESSAGING_RESPOND,
    SYS_MESSAGING_SUBSCRIBE,
    SYS_MESSAGING_UNSUBSCRIBE,

    SYS_FILESYSTEM_OPEN,
    SYS_FILESYSTEM_CLOSE,
    SYS_FILESYSTEM_READ,
    SYS_FILESYSTEM_WRITE,
    SYS_FILESYSTEM_IOCTL,
    SYS_FILESYSTEM_SEEK,
    SYS_FILESYSTEM_TELL,
    SYS_FILESYSTEM_STAT,
    SYS_FILESYSTEM_LINK,
    SYS_FILESYSTEM_UNLINK,
    SYS_FILESYSTEM_RENAME,
    SYS_FILESYSTEM_MKFIFO,
    SYS_FILESYSTEM_MKDIR,

    SYS_SYSTEM_GET_INFO,
    SYS_SYSTEM_GET_STATUS,

    SYSCALL_COUNT
} syscall_t;

static inline int __syscall(syscall_t syscall, int p1, int p2, int p3, int p4, int p5)
{
    int __ret;
    __asm__ __volatile__("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx"
                         : "=a"(__ret)
                         : "0"(syscall), "r"((int)(p1)), "c"((int)(p2)), "d"((int)(p3)), "S"((int)(p4)), "D"((int)(p5)));
    return __ret;
}

#define DECL_SYSCALL0(fn) int fn()
#define DECL_SYSCALL1(fn, p1) int fn(p1)
#define DECL_SYSCALL2(fn, p1, p2) int fn(p1, p2)
#define DECL_SYSCALL3(fn, p1, p2, p3) int fn(p1, p2, p3)
#define DECL_SYSCALL4(fn, p1, p2, p3, p4) int fn(p1, p2, p3, p4)
#define DECL_SYSCALL5(fn, p1, p2, p3, p4, p5) int fn(p1, p2, p3, p4, p5)

#define DECL_SYSCALL0(fn) int fn()
#define DECL_SYSCALL1(fn, p1) int fn(p1)
#define DECL_SYSCALL2(fn, p1, p2) int fn(p1, p2)
#define DECL_SYSCALL3(fn, p1, p2, p3) int fn(p1, p2, p3)
#define DECL_SYSCALL4(fn, p1, p2, p3, p4) int fn(p1, p2, p3, p4)
#define DECL_SYSCALL5(fn, p1, p2, p3, p4, p5) int fn(p1, p2, p3, p4, p5)

#define DEFN_SYSCALL0(fn, num)                \
    int fn()                                  \
    {                                         \
        return __syscall(num, 0, 0, 0, 0, 0); \
    }

#define DEFN_SYSCALL1(fn, num, P1)                  \
    int fn(P1 p1)                                   \
    {                                               \
        return __syscall(num, (int)p1, 0, 0, 0, 0); \
    }

#define DEFN_SYSCALL2(fn, num, P1, P2)                    \
    int fn(P1 p1, P2 p2)                                  \
    {                                                     \
        return __syscall(num, (int)p1, (int)p2, 0, 0, 0); \
    }

#define DEFN_SYSCALL3(fn, num, P1, P2, P3)                      \
    int fn(P1 p1, P2 p2, P3 p3)                                 \
    {                                                           \
        return __syscall(num, (int)p1, (int)p2, (int)p3, 0, 0); \
    }

#define DEFN_SYSCALL4(fn, num, P1, P2, P3, P4)                        \
    int fn(P1 p1, P2 p2, P3 p3, P4 p4)                                \
    {                                                                 \
        return __syscall(num, (int)p1, (int)p2, (int)p3, (int)p4, 0); \
    }

#define DEFN_SYSCALL5(fn, num, P1, P2, P3, P4, P5)                          \
    int fn(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)                               \
    {                                                                       \
        return __syscall(num, (int)p1, (int)p2, (int)p3, (int)p4, (int)p5); \
    }
