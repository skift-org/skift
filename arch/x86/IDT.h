#pragma once

#include <libsystem/Common.h>

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

#define IDT_ENTRY(__offset, __selector, __type)            \
    (IDTEntry)                                             \
    {                                                      \
        .offset0_15 = (u16)((__offset)&0xffff),            \
        .selector = (__selector),                          \
        .zero = 0,                                         \
        .type_attr = (__type),                             \
        .offset16_31 = (u16)(((__offset) >> 16) & 0xffff), \
    }

#ifdef __cplusplus
extern "C" void idt_flush(u32);
#else
extern void idt_flush(u32);
#endif
