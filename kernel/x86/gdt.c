/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */


#include "x86/gdt.h"

static tss_t tss = {.ss0 = 0x10, .esp0 = 0, .cs = 0x0b, .ss = 0x13, .ds = 0x13, .es = 0x13, .fs = 0x13, .gs = 0x13};

#define GDT_ENTRY(__base, __limit, __access, __flags) \
    (gdt_entry_t)                                     \
    {                                                 \
        .acces = (__access),                          \
        .flags = (__flags),                           \
        .base0_15 = (__base)&0xffff,                  \
        .base16_23 = ((__base) >> 16) & 0xff,         \
        .base24_31 = ((__base) >> 24) & 0xff,         \
        .limit0_15 = (__limit)&0xffff,                \
        .limit16_19 = ((__limit) >> 16) & 0x0f,       \
    }

static gdt_entry_t gdt_entries[GDT_ENTRY_COUNT];

static gdt_descriptor_t gdt_descriptor = {
    .size = sizeof(gdt_entry_t) * GDT_ENTRY_COUNT,
    .offset = (u32)&gdt_entries[0],
};

extern void gdt_flush(u32);

void gdt_setup()
{
    gdt_entries[0] = GDT_ENTRY(0, 0, 0, 0);
    gdt_entries[1] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_EXECUTABLE, GDT_FLAGS);
    gdt_entries[2] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE, GDT_FLAGS);
    gdt_entries[3] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_USER | GDT_EXECUTABLE, GDT_FLAGS);
    gdt_entries[4] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_USER, GDT_FLAGS);
    gdt_entries[5] = GDT_ENTRY(((uintptr_t)&tss), sizeof(tss_t), GDT_PRESENT | GDT_EXECUTABLE | GDT_ACCESSED, TSS_FLAGS);

    gdt_flush((u32)&gdt_descriptor);
}

void set_kernel_stack(u32 stack)
{
    tss.esp0 = stack;
}
