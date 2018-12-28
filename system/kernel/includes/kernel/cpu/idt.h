#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#include "kernel/processor.h"

// Gate call protection. 
// Specifies which privilege Level the calling Descriptor minimum should have. 
#define DPL_KERNEL 0b00000000 // ring 0
#define DPL_USER   0b01100000 // ring 1

#define INTGATE  0x8E
#define TRAPGATE 0x8F
#define IDT_ENTRY_COUNT 256

typedef void (*int_handler_t)(processor_context_t * states);

typedef PACKED(struct) 
{
    u16 size;
    u32 offset;
} idt_descriptor_t;

typedef PACKED(struct) 
{
    u16 offset0_15;  // offset bits 0..15
    u16 selector;    // a code segment selector in GDT or LDT
    u8 zero;
    u8 type_attr;   // type and attributes
    u16 offset16_31; // offset bits 16..31
} idt_entry_t;

typedef PACKED(struct) 
{
    idt_entry_t entries[IDT_ENTRY_COUNT];
    idt_descriptor_t descriptor;
    int_handler_t handlers[IDT_ENTRY_COUNT];
} idt_t;

void pic_setup();
void idt_setup();
void idt_entry(u8 index, u32 offset, u16 selector, u16 type);