#include "kernel/syscalls.h"
#include "cpu/irq.h"

syscalls_t syscalls[SYSCALL_COUNT];

void syscalls_setup()
{

}

void syscalls_handler(context_t * context)
{
    int syscall_number = context->eax;
    syscalls_t syscall = syscalls[context->eax];

    if (syscall_number > 0 &&
        syscall_number < SYSCALL_COUNT &&
        syscall != NULL)
    {
        context->eax = syscalls[context->eax](context->ebx, context->ecx, context->edx, context->esi, context->edi);
    }
    else
    {
        context->eax = (u32)NULL;
    }
}