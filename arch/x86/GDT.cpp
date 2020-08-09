
#include "arch/x86/GDT.h"

static TSS tss = {};

static GDTEntry gdt_entries[GDT_ENTRY_COUNT];

static GDTDescriptor gdt_descriptor = {
    .size = sizeof(GDTEntry) * GDT_ENTRY_COUNT,
    .offset = (uint32_t)&gdt_entries[0],
};

void gdt_initialize()
{
    tss.ss0 = 0x10;
    tss.esp0 = 0;
    tss.cs = 0x0b;
    tss.ss = 0x13;
    tss.ds = 0x13;
    tss.es = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;

    gdt_entries[0] = GDT_ENTRY(0, 0, 0, 0);

    gdt_entries[1] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_EXECUTABLE, GDT_FLAGS);
    gdt_entries[2] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE, GDT_FLAGS);

    gdt_entries[3] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_USER | GDT_EXECUTABLE, GDT_FLAGS);
    gdt_entries[4] = GDT_ENTRY(0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_USER, GDT_FLAGS);

    gdt_entries[5] = GDT_ENTRY(((uintptr_t)&tss), sizeof(TSS), GDT_PRESENT | GDT_EXECUTABLE | GDT_ACCESSED, TSS_FLAGS);

    gdt_flush((uint32_t)&gdt_descriptor);
}

void set_kernel_stack(uint32_t stack)
{
    tss.esp0 = stack;
}
