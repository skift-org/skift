#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

typedef enum
{
    SYS_DEBUG = 0,

/* --- Process and Threads -------------------------------------------------- */

    // Process
    SYS_PROCESS_SELF,

    SYS_PROCESS_EXEC,
    SYS_PROCESS_EXIT,
    SYS_PROCESS_CANCEL,

    // Process memory managment
    SYS_PROCESS_MAP,
    SYS_PROCESS_UNMAP,
    SYS_PROCESS_ALLOC,
    SYS_PROCESS_FREE,

    // Threads
    SYS_THREAD_SELF,
    SYS_THREAD_CREATE,
    SYS_THREAD_EXIT,
    SYS_THREAD_CANCEL,

    SYS_THREAD_SLEEP,
    SYS_THREAD_WAKEUP,

    SYS_THREAD_WAIT_THREAD,
    SYS_THREAD_WAIT_PROCESS,

    // Messaging
    SYS_MSG_SEND,
    SYS_MSG_BROADCAST,
    SYS_MSG_RECEIVE,
    SYS_MSG_PAYLOAD,

    SYS_MSG_SUBSCRIBE,
    SYS_MSG_UNSUBSCRIBE,

/* --- I/O ------------------------------------------------------------------ */
    
    SYS_IO_PRINT,
    SYS_IO_READ,

/* --- Filesystem ----------------------------------------------------------- */

    SYS_FILESYSTEM_OPEN,
    SYS_FILESYSTEM_OPENAT,
    
    SYS_FILESYSTEM_CLOSE,

    SYS_FILESYSTEM_READ,
    SYS_FILESYSTEM_WRITE,
    SYS_FILESYSTEM_FSTAT,
    SYS_FILESYSTEM_SEEK,
    SYS_FILESYSTEM_TELL,

    SYS_FILESYSTEM_MKDIR,
    SYS_FILESYSTEM_MKDIRAT,
    
    SYS_FILESYSTEM_LINK,
    SYS_FILESYSTEM_LINKAT,

    SYS_FILESYSTEM_UNLINK,
    SYS_FILESYSTEM_UNLINKAT,

////////////////////////////////////////////////////////////////////////////////

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