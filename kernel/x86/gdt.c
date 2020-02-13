/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/x86/gdt.h"

static TSS tss = {
    .ss0 = 0x10,
    .esp0 = 0,
    .cs = 0x0b,
    .ss = 0x13,
    .ds = 0x13,
    .es = 0x13,
    .fs = 0x13,
    .gs = 0x13,
};

static GDTEntry gdt_entries[GDT_ENTRY_COUNT];

static GDTDescriptor gdt_descriptor = {
    .size = sizeof(GDTEntry) * GDT_ENTRY_COUNT,
    .offset = (u32)&gdt_entries[0],
};

void gdt_setup()
{
    gdt_entries[0] = GDT_ENTRY(0, 0, 0, 0);

    gdt_entries[1] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_EXECUTABLE, GDT_FLAGS);
    gdt_entries[2] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE, GDT_FLAGS);

    gdt_entries[3] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_USER | GDT_EXECUTABLE, GDT_FLAGS);
    gdt_entries[4] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_USER, GDT_FLAGS);

    gdt_entries[5] = GDT_ENTRY(((uintptr_t)&tss), sizeof(TSS), GDT_PRESENT | GDT_EXECUTABLE | GDT_ACCESSED, TSS_FLAGS);

    gdt_flush((u32)&gdt_descriptor);
}

void set_kernel_stack(u32 stack)
{
    tss.esp0 = stack;
}
