/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include "kernel/tasking.h"
#include "kernel/syscalls.h"
#include "kernel/logger.h"

int sys_exit(int code)
{
    thread_exit();
    return code;
}

int sys_print(const char *string)
{
    printf(string);
    return 0;
}

int sys_map(int memory, int size)
{
    STUB(memory, size);
    return 0;
}

int sys_unmap(int memory, int size)
{
    STUB(memory, size);
    return 0;
}

typedef int (*syscall_t)(int, int, int, int, int);

static int (*syscalls[])() =
{
        [0] = sys_exit,
        [1] = sys_print,
        [2] = sys_map,
        [3] = sys_unmap,
};

void syscall_dispatcher(context_t *context)
{
    // log("syscall %d (EBX=%d, ECX=%d, EDX=%d, ESI=%d, EDI=%d).", context->eax, context->ebx, context->ecx, context->edx, context->esi, context->edi);

    syscall_t syscall = (syscall_t)syscalls[context->eax];
    context->eax = syscall(context->ebx, context->ecx, context->edx, context->esi, context->edi);
}