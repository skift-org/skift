#include "archs/x86_64/GDT.h"

namespace Arch::x86_64
{

static TSS64 tss = {
    .reserved = 0,
    .rsp = {},
    .reserved0 = 0,
    .ist = {},
    .reserved1 = 0,
    .reserved2 = 0,
    .reserved3 = 0,
    .iopb_offset = 0,
};

static GDT64 gdt = {};

static GDTDescriptor64 gdt_descriptor = {
    .size = sizeof(GDT64) - 1,
    .offset = (uint64_t)&gdt,
};

void gdt_initialize()
{
    gdt.entries[0] = {0, 0, 0, 0}; // null descriptor
    gdt.entries[1] = {GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE, GDT_LONG_MODE_GRANULARITY};
    gdt.entries[2] = {GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE, 0};

    gdt.entries[3] = {GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE | GDT_USER, GDT_LONG_MODE_GRANULARITY};
    gdt.entries[4] = {GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_USER, 0};

    gdt.tss = {(uintptr_t)&tss};

    gdt_flush((uint64_t)&gdt_descriptor);
}

void set_kernel_stack(uint64_t stack)
{
    tss.rsp[0] = stack;
    tss.ist[0] = stack;
}

} // namespace Arch::x86_64
