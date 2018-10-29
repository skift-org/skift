/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

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
    printf("\t GS=%x  FS=%x  ES=%x  DS=%x\n", context->gs,  context->fs,  context->es,  context->ds);
    printf("\tEDI=%x ESI=%x EBP=%x ESP=%x\n", context->edi, context->esi, context->ebp, context->USELESS);
    printf("\tEAX=%x EBX=%x ECX=%x EDX=%x\n", context->eax, context->ebx, context->ecx, context->edx);
    printf("\tINTNO=%x ERRCODE=%x EIP=%x CS=%x FLAGS=%x\n", context->int_no, context->errcode, context->eip, context->cs, context->eflags);

    printf("\tCR0=%x CR2=%x CR3=%x CR4=%x\n", CR0(), CR2(), CR3(), CR4());
}
