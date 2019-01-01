#pragma once

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

    SYS_THREAD_WAIT,
    SYS_THREAD_WAITPROC,

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

    //XXX: stop using mouse_syscalls
    SYS_IO_MOUSE_GET_STATE,
    SYS_IO_MOUSE_SET_STATE,

    SYS_IO_GRAPHIC_BLIT,
    SYS_IO_GRAPHIC_BLIT_REGION,
    SYS_IO_GRAPHIC_SIZE,

    /* --- Filesystem ----------------------------------------------------------- */

    // Files
    SYS_FILE_CREATE,
    SYS_FILE_DELETE,
    SYS_FILE_EXISTE,

    SYS_FILE_COPY,
    SYS_FILE_MOVE,

    SYS_FILE_STAT,

    SYS_FILE_OPEN,
    SYS_FILE_CLOSE,

    SYS_FILE_READ,
    SYS_FILE_WRITE,
    SYS_FILE_IOCTL,

    // Directories
    SYS_DIR_CREATE,
    SYS_DIR_DELETE,
    SYS_DIR_EXISTE,

    SYS_DIR_COPY,
    SYS_DIR_MOVE,

    SYS_DIR_OPEN,
    SYS_DIR_CLOSE,

    SYS_DIR_LISTFILE,
    SYS_DIR_LISTDIR,

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