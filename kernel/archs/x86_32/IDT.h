#pragma once

#include <libutils/Prelude.h>

namespace Arch::x86_32
{

#define INTGATE 0x8e
#define TRAPGATE 0xeF

#define IDT_USER 0b01100000

#define IDT_ENTRY_COUNT 256

struct PACKED IDTDescriptor
{
    uint16_t size;
    uint32_t offset;
};

struct PACKED IDTEntry
{
    uint16_t offset0_15; // offset bits 0..15
    uint16_t selector;   // a code segment selector in GDT or LDT
    uint8_t zero;
    uint8_t type_attr;    // type and attributes
    uint16_t offset16_31; // offset bits 16..31
};

#define IDT_ENTRY(__offset, __selector, __type)                 \
    (IDTEntry)                                                  \
    {                                                           \
        .offset0_15 = (uint16_t)((__offset)&0xffff),            \
        .selector = (__selector),                               \
        .zero = 0,                                              \
        .type_attr = (__type),                                  \
        .offset16_31 = (uint16_t)(((__offset) >> 16) & 0xffff), \
    }

extern "C" void idt_flush(uint32_t);

void idt_initialize();

} // namespace Arch::x86_32
