#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

typedef enum
{
    SYS_DEBUG = 0,

    SYS_PROCESS_THIS,
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
    SYS_FILESYSTEM_CALL,
    SYS_FILESYSTEM_SEEK,
    SYS_FILESYSTEM_TELL,
    SYS_FILESYSTEM_STAT,
    SYS_FILESYSTEM_LINK,
    SYS_FILESYSTEM_UNLINK,
    SYS_FILESYSTEM_RENAME,
    SYS_FILESYSTEM_MKPIPE,
    SYS_FILESYSTEM_MKDIR,

    SYS_SYSTEM_GET_INFO,
    SYS_SYSTEM_GET_STATUS,
    SYS_SYSTEM_GET_TIME,
    SYS_SYSTEM_GET_TICKS,

    SYS_HANDLE_OPEN,
    SYS_HANDLE_CLOSE,
    SYS_HANDLE_CONNECT,
    SYS_HANDLE_ACCEPT,
    SYS_HANDLE_SEND,
    SYS_HANDLE_RECEIVE,
    SYS_HANDLE_PAYLOAD,
    SYS_HANDLE_DISCARD,

    SYSCALL_COUNT
} syscall_t;

static inline int __syscall(syscall_t syscall, int p1, int p2, int p3, int p4, int p5)
{
    int __ret;
    __asm__ __volatile__("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx"
                         : "=a"(__ret)
                         : "0"(syscall), "r"((int)(p1)), "c"((int)(p2)), "d"((int)(p3)), "S"((int)(p4)), "D"((int)(p5))
                         : "memory");
    return __ret;
}
