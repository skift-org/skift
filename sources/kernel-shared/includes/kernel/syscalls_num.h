#pragma once

typedef enum SYSCALL_CODE
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

/* --- I/O ------------------------------------------------------------------ */

    SYS_IO_PRINT,
    SYS_IO_READ,

    SYS_IO_MOUSE_GET_POSITION,
    SYS_IO_MOUSE_SET_POSITION,

    SYS_IO_GRAPHIC_BLIT,
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
    SYS_DIR_LISTDIR
} syscall_code_t;