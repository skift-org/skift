#pragma once

#include <karm-base/enum.h>
#include <karm-base/result.h>
#include <karm-base/string.h>

namespace Hjert::Arch::x86_64 {

struct [[gnu::packed]] GdtEntry {
    uint16_t _limit_0_15;
    uint16_t _base_0_15;
    uint8_t _base_16_23;
    uint8_t _flags;
    uint8_t _limit_16_19 : 4;
    uint8_t _granularity : 4;
    uint8_t _base_24_31;

    constexpr GdtEntry(){};
    constexpr GdtEntry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags)
        : _limit_0_15(limit & 0xffff),
          _base_0_15(base & 0xffff),
          _base_16_23((base >> 16) & 0xff),
          _flags(flags),
          _limit_16_19((limit >> 16) & 0x0f),
          _granularity(granularity),
          _base_24_31((base >> 24) & 0xff) {}

    constexpr GdtEntry(uint8_t flags, uint8_t granularity) : _flags(flags), _granularity(granularity){};
};

struct [[gnu::packed]] Gdt {
    static constexpr int entry_count = 5;

    enum Section {
        KERNEL_CODE = 1,
        KERNEL_DATA = 2,
        USER_DATA = 3,
        USER_CODE = 4,
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

    Karm::Array<GdtEntry, Gdt::entry_count> _entries;

    Gdt() {
        _entries[0] = GdtEntry(0, 0, 0, 0);
        _entries[Gdt::KERNEL_CODE] = GdtEntry(Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::EXECUTABLE, Granularity::LONG_MODE);
        _entries[Gdt::KERNEL_DATA] = GdtEntry(Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE, 0);

        _entries[Gdt::USER_DATA] = GdtEntry(Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::USER, 0);
        _entries[Gdt::USER_CODE] = GdtEntry(Gdt::Flags::PRESENT | Gdt::Flags::SEGMENT | Gdt::Flags::READ_WRITE | Gdt::Flags::EXECUTABLE | Gdt::Flags::USER, Granularity::LONG_MODE);
    };
};

struct [[gnu::packed]] GdtDesc {

    uint16_t _limit;
    uint64_t _base;

    GdtDesc() : _limit(0), _base(0){};
    GdtDesc(uint16_t limit, Gdt *base) : _limit(limit), _base(reinterpret_cast<uintptr_t>(base)){};
};

void gdtInitialize();

} // namespace Hjert::Arch::x86_64
