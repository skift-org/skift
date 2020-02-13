#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define INTGATE 0x8e
#define TRAPGATE 0xeF

#define IDT_ENTRY_COUNT 256

typedef struct __packed
{
    u16 size;
    u32 offset;
} IDTDescriptor;

typedef struct __packed
{
    u16 offset0_15; // offset bits 0..15
    u16 selector;   // a code segment selector in GDT or LDT
    u8 zero;
    u8 type_attr;    // type and attributes
    u16 offset16_31; // offset bits 16..31
} IDTEntry;

#define IDT_ENTRY(__offset, __selector, __type)     \
    (IDTEntry)                                      \
    {                                               \
        .offset0_15 = (__offset)&0xffff,            \
        .offset16_31 = ((__offset) >> 16) & 0xffff, \
        .zero = 0,                                  \
        .selector = (__selector),                   \
        .type_attr = (__type),                      \
    }

void idt_flush(u32);
