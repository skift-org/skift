#pragma once

/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#ifndef USERSPACE

#include "types.h"
#include "cpu/cpu.h"

#endif

/* --- Processes managment -------------------------------------------------- */

#define SYS_PROCESS_SELF __COUNTER__

#define SYS_PROCESS_EXEC __COUNTER__
#define SYS_PROCESS_EXIT __COUNTER__
#define SYS_PROCESS_CANCEL __COUNTER__

#define SYS_PROCESS_MAP __COUNTER__
#define SYS_PROCESS_UNMAP __COUNTER__

/* --- Thread managment ----------------------------------------------------- */

#define SYS_THREAD_SELF __COUNTER__

#define SYS_THREAD_CREATE __COUNTER__
#define SYS_THREAD_EXIT __COUNTER__
#define SYS_THREAD_CANCEL __COUNTER__

#define SYS_THREAD_SLEEP __COUNTER__
#define SYS_THREAD_WAKEUP __COUNTER__

#define SYS_THREAD_WAIT __COUNTER__
#define SYS_THREAD_WAITPROC __COUNTER__

/* --- I/O ------------------------------------------------------------------ */

// CONSOLE
#define SYS_PRINT __COUNTER__
#define SYS_READ __COUNTER__

// FILES
#define SYS_FILE_CREATE __COUNTER__
#define SYS_FILE_DELETE __COUNTER__
#define SYS_FILE_EXISTE __COUNTER__

#define SYS_FILE_COPY __COUNTER__
#define SYS_FILE_MOVE __COUNTER__
#define SYS_FILE_STAT __COUNTER__

#define SYS_FILE_OPEN __COUNTER__
#define SYS_FILE_CLOSE __COUNTER__
#define SYS_FILE_READ __COUNTER__
#define SYS_FILE_WRITE __COUNTER__
#define SYS_FILE_IOCTL __COUNTER__

// DIRECTORIES
#define SYS_DIR_CREATE __COUNTER__
#define SYS_DIR_DELETE __COUNTER__
#define SYS_DIR_EXISTE __COUNTER__

#define SYS_DIR_OPEN __COUNTER__
#define SYS_DIR_CLOSE __COUNTER__
#define SYS_DIR_LISTFILE __COUNTER__
#define SYS_DIR_LISTDIR __COUNTER__

/* --- Kernel side handlers ------------------------------------------------- */

#ifndef USERSPACE

int sys_exit(int code);
int sys_print(const char *string);

void syscall_dispatcher(context_t *context);

#endif