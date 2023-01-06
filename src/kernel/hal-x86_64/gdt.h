#pragma once

#include <karm-base/array.h>

namespace x86_64 {

struct Tss {
    uint32_t _reserved;
    Array<uint64_t, 3> _rsp;
    uint64_t _reserved0;
    Array<uint64_t, 7> _ist;
    uint32_t _reserved1;
    uint32_t _reserved2;
    uint16_t _reserved3;
    uint16_t _iopbOffset;
};

struct [[gnu::packed]] GdtEntry {
    uint16_t _limitLow{};
    uint16_t _baseLow{};
    uint8_t _baseMid{};
    uint8_t _flags{};
    uint8_t _limitHigh : 4 {};
    uint8_t _granularity : 4 {};
    uint8_t _baseHigh{};

    constexpr GdtEntry() = default;

    constexpr GdtEntry(uint8_t flags, uint8_t granularity)
        : _flags(flags),
          _granularity(granularity){};

    constexpr GdtEntry(uint32_t base, uint32_t limit, uint8_t flags, uint8_t granularity)
        : _limitLow(limit & 0xffff),
          _baseLow(base & 0xffff),
          _baseMid((base >> 16) & 0xff),
          _flags(flags),
          _limitHigh((limit >> 16) & 0x0f),
          _granularity(granularity),
          _baseHigh((base >> 24) & 0xff) {}
};

struct GdtTssEntry {
    uint16_t _len;
    uint16_t _baseLow16;
    uint8_t _baseMid8;
    uint8_t _flags1;
    uint8_t _flags2;
    uint8_t _baseHigh8;
    uint32_t _baseUpper32;
    uint32_t _reserved;

    constexpr GdtTssEntry() = default;

    GdtTssEntry(Tss const &tss)
        : _len(sizeof(Tss)),
          _baseLow16((uintptr_t)&tss & 0xffff),
          _baseMid8(((uintptr_t)&tss >> 16) & 0xff),
          _flags1(0b10001001),
          _flags2(0),
          _baseHigh8(((uintptr_t)&tss >> 24) & 0xff),
          _baseUpper32((uintptr_t)&tss >> 32),
          _reserved() {}
};

struct [[gnu::packed]] Gdt {
    static constexpr int LEN = 5;

    enum Selector {
        ZERO = 0,
        KCODE = 1,
        KDATA = 2,
        UDATA = 3,
        UCODE = 4,
        TSS = 5, /* not implemented */
    };

    enum Flags : uint32_t {
        SEGMENT = 0b00010000,
        PRESENT = 0b10000000,
        USER = 0b01100000,
        EXECUTABLE = 0b00001000,
        READ_WRITE = 0b00000010,
    };

    enum Granularity {
        LONG_MODE = 0b10,
    };

    Karm::Array<GdtEntry, Gdt::LEN> entries = {
        GdtEntry{},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::EXECUTABLE, Granularity::LONG_MODE},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE, 0},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::USER, 0},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::EXECUTABLE | Gdt::Flags::USER, Granularity::LONG_MODE},
    };
};

extern "C" void _gdtLoad(void const *ptr); // implemented in gdt.s

struct [[gnu::packed]] GdtDesc {
    uint16_t _limit;
    uint64_t _base;

    GdtDesc(Gdt const &base)
        : _limit(sizeof(Gdt) - 1),
          _base(reinterpret_cast<uintptr_t>(&base)) {}

    void load() const { _gdtLoad(this); }
};

} // namespace x86_64
