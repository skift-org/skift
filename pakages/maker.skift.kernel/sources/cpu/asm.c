/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* asm.c: x86 assembly helpers.                                               */

#include <stdio.h>

#include "kernel/cpu/cpu.h"

u8 inb(u16 port)
{
    u8 data;
    asm volatile("in %1,%0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

void outb(u16 port, u8 data)
{
    asm volatile("out %0,%1"
                 :
                 : "a"(data), "d"(port));
}

u16 inw(u16 port)
{
    u16 data;
    asm volatile("inw %1,%0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

void outw(u16 port, u16 data)
{
    asm volatile("outw %0,%1"
                 :
                 : "a"(data), "d"(port));
}

u32 CR0()
{
    u32 r;
    asm volatile("mov %%cr0, %0"
                 : "=r"(r));
    return r;
}

u32 CR2()
{
    u32 r;
    asm volatile("mov %%cr2, %0"
                 : "=r"(r));
    return r;
}

u32 CR3()
{
    u32 r;
    asm volatile("mov %%cr3, %0"
                 : "=r"(r));
    return r;
}

u32 CR4()
{
    u32 r;
    asm volatile("mov %%cr4, %0"
                 : "=r"(r));
    return r;
}

void dump_context(context_t *context)
{
    printf("\t CS=%08x  DS=%08x  ES=%08x  FS=%08x  GS=%08x\n", context->cs,  context->ds,  context->es,  context->fs, context->gs);
    printf("\tEAX=%08x EBX=%08x ECX=%08x EDX=%08x\n", context->eax, context->ebx, context->ecx, context->edx);
    printf("\tEDI=%08x ESI=%08x EBP=%08x ESP=%08x\n", context->edi, context->esi, context->ebp, context->USELESS);
    printf("\tINT=%08x ERR=%08x EIP=%08x FLG=%08x\n", context->int_no, context->errcode, context->eip, context->eflags);

    printf("\tCR0=%08x CR2=%08x CR3=%08x CR4=%08x\n", CR0(), CR2(), CR3(), CR4());
}
