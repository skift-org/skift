#pragma once

#include <karm-base/enum.h>
#include <karm-base/result.h>
#include <karm-base/string.h>

namespace x86_64 {

struct [[gnu::packed]] GdtSeg {
    uint16_t _limitLow;
    uint16_t _baseLow;
    uint8_t _baseMid;
    uint8_t _flags;
    uint8_t _limitHigh : 4;
    uint8_t _granularity : 4;
    uint8_t _baseHigh;

    constexpr GdtSeg() = default;

    constexpr GdtSeg(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags)
        : _limitLow(limit & 0xffff),
          _baseLow(base & 0xffff),
          _baseMid((base >> 16) & 0xff),
          _flags(flags),
          _limitHigh((limit >> 16) & 0x0f),
          _granularity(granularity),
          _baseHigh((base >> 24) & 0xff) {}

    constexpr GdtSeg(uint8_t flags, uint8_t granularity) : _flags(flags), _granularity(granularity){};
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

    Karm::Array<GdtSeg, Gdt::LEN> _entries = {
        {},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::EXECUTABLE, Granularity::LONG_MODE},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE, 0},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::USER, 0},
        {Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::EXECUTABLE | Gdt::Flags::USER, Granularity::LONG_MODE},
    };
};

extern "C" void _gdtLoad(void *ptr); // implemented in gdt.s

struct [[gnu::packed]] GdtDesc {
    uint64_t _base{};
    uint16_t _limit{};

    GdtDesc() = default;

    GdtDesc(Gdt &base) : _base(reinterpret_cast<uintptr_t>(&base)), _limit(sizeof(Gdt) - 1){};

    void load() { _gdtLoad(this); }
};

void gdtInitialize();

} // namespace x86_64
