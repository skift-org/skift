#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#define cli() asm volatile("cli")
#define sti() asm volatile("sti")
#define hlt() asm volatile("hlt")

u32 CR0();
u32 CR2();
u32 CR3();
u32 CR4();

u8   inb (u16 port);
void outb(u16 port, u8 data);
u16  inw (u16 port);
void outw(u16 port, u16 data);

typedef PACKED(struct)
{
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, USELESS, ebx, edx, ecx, eax;
    u32 int_no, errcode;
    u32 eip, cs, eflags;
} context_t;

void dump_context(context_t* context);
