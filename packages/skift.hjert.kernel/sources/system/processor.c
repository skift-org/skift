/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include "kernel/processor.h"

/* --- Public functions ----------------------------------------------------- */

void processor_setup()
{
    // setup the gdt and idt
}

void processor_dump_context(processor_context_t *context)
{
    printf("\tCS=%04x DS=%04x ES=%04x FS=%04x GS=%04x\n", context->cs,  context->ds,  context->es,  context->fs, context->gs);
    printf("\tEAX=%08x EBX=%08x ECX=%08x EDX=%08x\n", context->eax, context->ebx, context->ecx, context->edx);
    printf("\tEDI=%08x ESI=%08x EBP=%08x ESP=%08x\n", context->edi, context->esi, context->ebp, context->USELESS);
    printf("\tINT=%08x ERR=%08x EIP=%08x FLG=%08x\n", context->int_no, context->errcode, context->eip, context->eflags);

    printf("\tCR0=%08x CR2=%08x CR3=%08x CR4=%08x\n", CR0(), CR2(), CR3(), CR4());
}