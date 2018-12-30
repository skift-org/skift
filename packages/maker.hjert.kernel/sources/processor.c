#include <stdio.h>
#include "kernel/processor.h"

/* --- Public functions ----------------------------------------------------- */

void processor_setup()
{

}

void processor_dump_context(processor_context_t *context)
{
    printf("\t CS=%08x  DS=%08x  ES=%08x  FS=%08x  GS=%08x\n", context->cs,  context->ds,  context->es,  context->fs, context->gs);
    printf("\tEAX=%08x EBX=%08x ECX=%08x EDX=%08x\n", context->eax, context->ebx, context->ecx, context->edx);
    printf("\tEDI=%08x ESI=%08x EBP=%08x ESP=%08x\n", context->edi, context->esi, context->ebp, context->USELESS);
    printf("\tINT=%08x ERR=%08x EIP=%08x FLG=%08x\n", context->int_no, context->errcode, context->eip, context->eflags);

    printf("\tCR0=%08x CR2=%08x CR3=%08x CR4=%08x\n", CR0(), CR2(), CR3(), CR4());
}