#pragma once

#include <karm-base/array.h>

namespace x86_64 {

struct [[gnu::packed]] Tss {
    u32 _reserved;
    Array<u64, 3> _rsp;
    u64 _reserved0;
    Array<u64, 7> _ist;
    u32 _reserved1;
    u32 _reserved2;
    u16 _reserved3;
    u16 _iopbOffset;
};

struct [[gnu::packed]] GdtEntry {
    u16 _limitLow{};
    u16 _baseLow{};
    u8 _baseMid{};
    u8 _flags{};
    u8 _limitHigh : 4 {};
    u8 _granularity : 4 {};
    u8 _baseHigh{};

    constexpr GdtEntry() = default;

    constexpr GdtEntry(u8 flags, u8 granularity)
        : _flags(flags),
          _granularity(granularity){};

    constexpr GdtEntry(u32 base, u32 limit, u8 flags, u8 granularity)
        : _limitLow(limit & 0xffff),
          _baseLow(base & 0xffff),
          _baseMid((base >> 16) & 0xff),
          _flags(flags),
          _limitHigh((limit >> 16) & 0x0f),
          _granularity(granularity),
          _baseHigh((base >> 24) & 0xff) {}
};

struct [[gnu::packed]] GdtTssEntry {
    u16 _len;
    u16 _baseLow16;
    u8 _baseMid8;
    u8 _flags1;
    u8 _flags2;
    u8 _baseHigh8;
    u32 _baseUpper32;
    u32 _reserved;

    constexpr GdtTssEntry() = default;

    GdtTssEntry(Tss const &tss)
        : _len(sizeof(Tss)),
          _baseLow16((usize)&tss & 0xffff),
          _baseMid8(((usize)&tss >> 16) & 0xff),
          _flags1(0b10001001),
          _flags2(0),
          _baseHigh8(((usize)&tss >> 24) & 0xff),
          _baseUpper32((usize)&tss >> 32),
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

    Gdt(Tss const &tss)
        : tssEntry(tss) {}
};

extern "C" void _gdtLoad(void const *ptr); // implemented in gdt.s

extern "C" void _tssUpdate();

struct [[gnu::packed]] GdtDesc {
    u16 _limit;
    u64 _base;

    GdtDesc(Gdt const &base)
        : _limit(sizeof(Gdt) - 1),
          _base(reinterpret_cast<usize>(&base)) {}

    void load() const { _gdtLoad(this); }
};

} // namespace x86_64
