/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/tasking.h"
#include "kernel/syscalls.h"
#include "kernel/syscalls_num.h"
#include "kernel/logger.h"
#include "kernel/serial.h"

typedef int (*syscall_t)(int, int, int, int, int);

int sys_not_implemented()
{
    log("Not implemented syscall!");
    return -1;
}

/* --- Process -------------------------------------------------------------- */

int sys_process_self()
{
    return process_self();
}

int sys_process_exec(const char *file_name)
{
    return process_exec(file_name, NULL);
}

int sys_process_exit(int code)
{
    process_exit(code);
    
    return 0;
}

/* --- Threads -------------------------------------------------------------- */

int sys_thread_self()
{
    return thread_self();
}

/* --- System I/O ----------------------------------------------------------- */

int sys_io_print(const char *msg)
{
    log("[PID:%d] %s", process_self(), msg);

    return 0;
}

static int (*syscalls[])() =
    {
        [SYS_PROCESS_SELF] = sys_process_self,
        [SYS_PROCESS_EXEC] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_PROCESS_EXIT] = sys_process_exit,
        [SYS_PROCESS_CANCEL] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_PROCESS_MAP] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_PROCESS_UNMAP] = sys_not_implemented /* NOT IMPLEMENTED */,
        
        [SYS_THREAD_SELF] = sys_thread_self,
        [SYS_THREAD_CREATE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_THREAD_EXIT] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_THREAD_CANCEL] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_THREAD_SLEEP] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_THREAD_WAKEUP] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_THREAD_WAIT] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_THREAD_WAITPROC] = sys_not_implemented /* NOT IMPLEMENTED */,

        [SYS_IO_PRINT] = sys_io_print,
        [SYS_IO_READ] = sys_not_implemented /* NOT IMPLEMENTED */,
        
        [SYS_FILE_CREATE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_DELETE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_EXISTE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_COPY] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_MOVE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_STAT] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_OPEN] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_CLOSE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_READ] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_WRITE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_IOCTL] = sys_not_implemented /* NOT IMPLEMENTED */,

        [SYS_DIR_CREATE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_DELETE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_EXISTE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_OPEN] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_CLOSE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_LISTFILE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_LISTDIR] = sys_not_implemented /* NOT IMPLEMENTED */,
};

void syscall_dispatcher(context_t *context)
{
    //log("syscall %d (EBX=%d, ECX=%d, EDX=%d, ESI=%d, EDI=%d).", context->eax, context->ebx, context->ecx, context->edx, context->esi, context->edi);

    syscall_t syscall = (syscall_t)syscalls[context->eax];
    context->eax = syscall(context->ebx, context->ecx, context->edx, context->esi, context->edi);
}