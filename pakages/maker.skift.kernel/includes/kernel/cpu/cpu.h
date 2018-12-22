#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#define cli() asm volatile("cli")
#define sti() asm volatile("sti")
#define hlt() asm volatile("hlt")

typedef u32 reg32_t;

reg32_t CR0();
reg32_t CR2();
reg32_t CR3();
reg32_t CR4();

u8   inb (u16 port);
void outb(u16 port, u8 data);
u16  inw (u16 port);
void outw(u16 port, u16 data);

typedef PACKED(struct)
{
    reg32_t gs, fs, es, ds;
    reg32_t edi, esi, ebp, USELESS, ebx, edx, ecx, eax;
    reg32_t int_no, errcode;
    reg32_t eip, cs, eflags;
} context_t;

void dump_context(context_t* context);
