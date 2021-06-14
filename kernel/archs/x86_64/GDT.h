#pragma once

#include "system/Streams.h"
#include <libutils/Prelude.h>

namespace Arch::x86_64
{

#define GDT_ENTRY_COUNT 5

#define GDT_SEGMENT (0b00010000)
#define GDT_PRESENT (0b10000000)
#define GDT_USER (0b01100000)
#define GDT_EXECUTABLE (0b00001000)
#define GDT_READWRITE (0b00000010)

#define GDT_LONG_MODE_GRANULARITY 0b0010

#define GDT_FLAGS 0b1100

struct PACKED TSS64
{
    uint32_t reserved;

    uint64_t rsp[3];

    uint64_t reserved0;

    uint64_t ist[7];

    uint32_t reserved1;
    uint32_t reserved2;
    uint16_t reserved3;

    uint16_t iopb_offset;
};

struct PACKED GDTDescriptor64
{
    uint16_t size;
    uint64_t offset;
};

struct PACKED GDTEntry64
{
    uint16_t limit0_15;
    uint16_t base0_15;
    uint8_t base16_23;
    uint8_t flags;
    uint8_t limit16_19 : 4;
    uint8_t granularity : 4;
    uint8_t base24_31;

    constexpr GDTEntry64()
        : GDTEntry64(0, 0, 0, 0)
    {
    }

    constexpr GDTEntry64(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags)
        : limit0_15((uint16_t)((limit)&0xffff)),
          base0_15((uint16_t)((base)&0xffff)),
          base16_23((uint8_t)(((base) >> 16) & 0xff)),
          flags((flags)),
          limit16_19(((limit) >> 16) & 0x0f),
          granularity((granularity)),
          base24_31((uint8_t)(((base) >> 24) & 0xff))
    {
    }

    constexpr GDTEntry64(uint8_t flags, uint8_t granularity)
        : GDTEntry64(0, 0, granularity, flags)
    {
    }
};

struct PACKED GDTTSSEntry64
{
    uint16_t length;
    uint16_t base_low16;
    uint8_t base_mid8;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t base_high8;
    uint32_t base_upper32;
    uint32_t reserved;

    constexpr void address(uintptr_t addr)
    {
        base_low16 = (addr >> 0) & 0xffff;
        base_mid8 = (addr >> 16) & 0xff;
        base_high8 = (addr >> 24) & 0xff;
        base_upper32 = (addr >> 32) & 0xffffffff;
    }

    constexpr GDTTSSEntry64(uintptr_t tss_address)
        : length(sizeof(TSS64)),
          base_low16(tss_address & 0xffff),
          base_mid8((tss_address >> 16) & 0xff),
          flags1(0b10001001),
          flags2(0),
          base_high8((tss_address >> 24) & 0xff),
          base_upper32(tss_address >> 32),
          reserved(0)
    {
    }
};

struct PACKED GDT64
{
    GDTEntry64 entries[GDT_ENTRY_COUNT] = {};
    GDTTSSEntry64 tss = {0};
};

void gdt_initialize();

extern "C" void gdt_flush(uint64_t);

extern "C" void tss_flush(uint64_t);

void set_kernel_stack(uint64_t stack);

} // namespace Arch::x86_64
