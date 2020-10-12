#include "arch/x86_64/kernel/GDT.h"

static TSS64 tss = {
    .reserved = 0,
    .rsp0 = 0,
    .rsp1 = 0,
    .rsp2 = 0,
    .reserved0 = 0,
    .ist1 = 0,
    .ist2 = 0,
    .ist3 = 0,
    .ist4 = 0,
    .ist5 = 0,
    .ist6 = 0,
    .ist7 = 0,
    .reserved1 = 0,
    .reserved2 = 0,
    .reserved3 = 0,
    .iopb_offset = 0,
};

static GDT64 gdt = {};

static GDTDescriptor64 gdt_descriptor = {
    .size = sizeof(GDT64),
    .offset = (uint64_t)&gdt,
};

void gdt_initialize()
{
    gdt.entries[0] = {0, 0, 0, 0};

    gdt.entries[1] = {0, 0, GDT_PRESENT | GDT_READWRITE | GDT_EXECUTABLE, 0};
    gdt.entries[2] = {0, 0, GDT_PRESENT | GDT_READWRITE, 0};

    gdt.entries[3] = {0, 0, GDT_USER | GDT_PRESENT | GDT_READWRITE | GDT_EXECUTABLE, 0};
    gdt.entries[4] = {0, 0, GDT_USER | GDT_PRESENT | GDT_READWRITE, 0};

    gdt_flush((uint64_t)&gdt_descriptor);
}

void set_kernel_stack(uint64_t stack)
{
    tss.rsp0 = stack;
}
