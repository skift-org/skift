/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "x86/idt.h"

IDTEntry idt_entries[IDT_ENTRY_COUNT];

IDTDescriptor idt_descriptor = {
    .offset = (u32)&idt_entries[0],
    .size = sizeof(IDTEntry) * IDT_ENTRY_COUNT,
};

void idt_setup(void)
{
    pic_remap();
    idt_flush((u32)&idt_descriptor);
}

void idt_entry(int index, uintptr_t offset, u16 selector, u16 type)
{
    IDTEntry *entry = &idt_entries[index];

    entry->offset0_15 = offset & 0xffff;
    entry->offset16_31 = (offset >> 16) & 0xffff;

    entry->zero = 0;

    entry->selector = selector;
    entry->type_attr = type;
}
