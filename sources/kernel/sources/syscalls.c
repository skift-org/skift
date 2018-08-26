#include <stdio.h>
#include "kernel/syscalls.h"
#include "kernel/logging.h"

int sys_exit(int code)
{
    return code;
}

int sys_print(const char *string)
{
    printf(string);
    return 0;
}

void syscall_dispatcher(context_t *context)
{
    debug("syscall %d.", context->eax);

    switch (context->eax)
    {
    case 0:
        context->eax = sys_exit(context->ebx);
        break;

    case 1:
        context->eax = sys_print((const char *)context->ebx);
        break;

    default:
        break;
    }
}