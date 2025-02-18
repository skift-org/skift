#pragma once

#include <karm-base/array.h>

namespace x86_64 {

struct [[gnu::packed]] IdtEntry {
    u16 offsetLow{};
    u16 codeSegment{};
    u8 ist{};
    u8 attributes{};
    u16 offsetMid{};
    u32 offsetHigh{};
    u32 zero{};

    static constexpr u8 TRAP = 0xeF;
    static constexpr u8 USER = 0x60;
    static constexpr u8 GATE = 0x8e;

    IdtEntry() = default;

    IdtEntry(usize handler, u8 ist, u8 attributes)
        : offsetLow(handler & 0xffff),
          codeSegment(0x8),
          ist(ist),
          attributes(attributes),
          offsetMid((handler >> 16) & 0xffff),
          offsetHigh(handler >> 32) {}
};

struct [[gnu::packed]] Idt {
    static constexpr isize LEN = 256;

    Karm::Array<IdtEntry, LEN> entries{};
};

extern "C" void _idtLoad(void const* ptr);

struct [[gnu::packed]] IdtDesc {
    u16 limit;
    u64 base;

    IdtDesc(Idt const& base)
        : limit(sizeof(Idt) - 1),
          base(reinterpret_cast<usize>(&base)) {}

    void load() const {
        _idtLoad(this);
    }
};

} // namespace x86_64
