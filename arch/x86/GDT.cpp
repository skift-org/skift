#include "arch/x86/GDT.h"

static TSS tss = {
    .prev_tss = 0,
    .esp0 = 0,
    .ss0 = 0x10,
    .esp1 = 0,
    .ss1 = 0,
    .esp2 = 0,
    .ss2 = 0,
    .cr3 = 0,
    .eip = 0,
    .eflags = 0,
    .eax = 0,
    .ecx = 0,
    .edx = 0,
    .ebx = 0,
    .esp = 0,
    .ebp = 0,
    .esi = 0,
    .edi = 0,
    .es = 0,
    .cs = 0,
    .ss = 0,
    .ds = 0,
    .fs = 0,
    .gs = 0,
    .ldt = 0,
    .trap = 0,
    .iomap_base = 0,
};

static GDTEntry gdt_entries[GDT_ENTRY_COUNT] = {
    {0, 0, 0, 0},
    {0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_EXECUTABLE, GDT_FLAGS},
    {0, 0xffffffff, GDT_PRESENT | GDT_READWRITE, GDT_FLAGS},
    {0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_USER | GDT_EXECUTABLE, GDT_FLAGS},
    {0, 0xffffffff, GDT_PRESENT | GDT_READWRITE | GDT_USER, GDT_FLAGS},
    {0, 0, 0, 0},
};

static GDTDescriptor gdt_descriptor = {
    .size = sizeof(GDTEntry) * GDT_ENTRY_COUNT,
    .offset = (uint32_t)&gdt_entries[0],
};

void gdt_initialize()
{
    gdt_entries[5] = GDTEntry{(uintptr_t)&tss,
                              ((uintptr_t)&tss) + sizeof(TSS),
                              GDT_TSS_PRESENT | GDT_ACCESSED | GDT_EXECUTABLE | GDT_USER,
                              TSS_FLAGS};

    for (size_t i = 0; i < GDT_ENTRY_COUNT; i++)
    {
        gdt_entries[i].dump(i);
    }

    gdt_flush((uint32_t)&gdt_descriptor);
}

void set_kernel_stack(uint32_t stack)
{
    tss.esp0 = stack;
}
