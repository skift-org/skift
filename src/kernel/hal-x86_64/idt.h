#pragma once

#include <karm-base/array.h>

namespace x86_64 {

struct [[gnu::packed]] IdtEntry {
    u16 _offsetLow{};
    u16 _codeSegment{};
    u8 _ist{};
    u8 _attributes{};
    u16 _offsetMid{};
    u32 _offsetHigh{};
    u32 _zero{};

    static constexpr u8 TRAP = 0xeF;
    static constexpr u8 USER = 0x60;
    static constexpr u8 GATE = 0x8e;

    IdtEntry() = default;

    IdtEntry(usize handler, u8 ist, u8 attributes)
        : _offsetLow(handler & 0xffff),
          _codeSegment(0x8),
          _ist(ist),
          _attributes(attributes),
          _offsetMid((handler >> 16) & 0xffff),
          _offsetHigh(handler >> 32),
          _zero(0) {}
};

struct [[gnu::packed]] Idt {
    static constexpr isize LEN = 256;

    Karm::Array<IdtEntry, LEN> entries{};
};

extern "C" void _idtLoad(void const *ptr);

struct [[gnu::packed]] IdtDesc {
    u16 _limit;
    u64 _base;

    IdtDesc(Idt const &base)
        : _limit(sizeof(Idt) - 1),
          _base(reinterpret_cast<usize>(&base)) {}

    void load() const { _idtLoad(this); }
};

} // namespace x86_64
