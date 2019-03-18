/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include <string.h>

#include "kernel/cpu/idt.h"

idt_t idt;

// define in interupts.S
extern u32 int_handlers[];
extern void load_idt(u32);

void idt_setup()
{
    pic_setup();

    // cleanup the idt.
    memset(&idt, 0, sizeof(idt_t));

    // setup the descriptor.
    idt.descriptor.offset = (u32)&idt.entries;
    idt.descriptor.size = sizeof(idt_entry_t) * IDT_ENTRY_COUNT;

    // load the idt
    load_idt((u32)&idt.descriptor);
}

void idt_entry(u8 index, u32 offset, u16 selector, u16 type)
{
    // printf("IDT[%d]: OFFSET=0x%x SELECTOR=0x%x TYPE=%b\n", index, offset, selector, type);

    idt_entry_t *entry = &idt.entries[index];

    entry->offset0_15 = offset & 0xffff;
    entry->offset16_31 = (offset >> 16) & 0xffff;

    entry->zero = 0;

    entry->selector = selector;
    entry->type_attr = type;
}