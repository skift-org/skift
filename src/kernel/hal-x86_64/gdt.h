#pragma once

#include <karm-base/array.h>

namespace x86_64 {

struct [[gnu::packed]] Tss {
    u32 _reserved;
    Array<u64, 3> rsp;
    u64 _reserved1;
    Array<u64, 7> ist;
    u32 _reserved2;
    u32 _reserved3;
    u16 _reserved4;
    u16 iopbOffset;
};

struct [[gnu::packed]] GdtEntry {
    u16 limitLow{};
    u16 baseLow{};
    u8 baseMid{};
    u8 flags{};
    u8 limitHigh : 4 {};
    u8 granularity : 4 {};
    u8 baseHigh{};

    constexpr GdtEntry() = default;

    constexpr GdtEntry(u8 flags, u8 granularity)
        : flags(flags),
          granularity(granularity) {};

    constexpr GdtEntry(u32 base, u32 limit, u8 flags, u8 granularity)
        : limitLow(limit & 0xffff),
          baseLow(base & 0xffff),
          baseMid((base >> 16) & 0xff),
          flags(flags),
          limitHigh((limit >> 16) & 0x0f),
          granularity(granularity),
          baseHigh((base >> 24) & 0xff) {}
};

struct [[gnu::packed]] GdtTssEntry {
    u16 len;
    u16 baseLow16;
    u8 baseMid8;
    u8 flags1;
    u8 flags2;
    u8 baseHigh8;
    u32 baseUpper32;
    u32 _reserved;

    constexpr GdtTssEntry() = default;

    GdtTssEntry(Tss const& tss)
        : len(sizeof(Tss)),
          baseLow16((usize)&tss & 0xffff),
          baseMid8(((usize)&tss >> 16) & 0xff),
          flags1(0b10001001),
          flags2(0),
          baseHigh8(((usize)&tss >> 24) & 0xff),
          baseUpper32((usize)&tss >> 32),
          _reserved() {}
};

struct [[gnu::packed]] Gdt {
    static constexpr usize LEN = 6;

    enum Selector {
        ZERO = 0,
        KCODE = 1,
        KDATA = 2,
        UDATA = 3,
        UCODE = 4,
        TSS = 5,
    };

    enum Flags : u32 {
        SEGMENT = 0b00010000,
        PRESENT = 0b10000000,
        USER = 0b01100000,
        EXECUTABLE = 0b00001000,
        READ_WRITE = 0b00000010,
    };

    enum Granularity {
        LONG_MODE = 0b10,
    };

    Karm::Array<GdtEntry, Gdt::LEN - 1> entries = {
        GdtEntry{},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::EXECUTABLE, Granularity::LONG_MODE},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE, 0},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::USER, 0},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::EXECUTABLE | Gdt::Flags::USER, Granularity::LONG_MODE},
    };

    GdtTssEntry tssEntry;

    Gdt(Tss const& tss)
        : tssEntry(tss) {}
};

extern "C" void _gdtLoad(void const* ptr);

extern "C" void _tssUpdate();

struct [[gnu::packed]] GdtDesc {
    u16 limit;
    u64 base;

    GdtDesc(Gdt const& base)
        : limit(sizeof(Gdt) - 1),
          base(reinterpret_cast<usize>(&base)) {}

    void load() const { _gdtLoad(this); }
};

} // namespace x86_64
