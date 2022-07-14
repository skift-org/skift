#pragma once

#include <karm-base/array.h>

namespace x86_64 {

struct [[gnu::packed]] IdtEntry {
    uint16_t _offsetLow{};
    uint16_t _codeSegment{};
    uint8_t _ist{};
    uint8_t _attributes{};
    uint16_t _offsetMid{};
    uint32_t _offsetHigh{};
    uint32_t _zero{};

    static constexpr uint8_t TRAP = 0xeF;
    static constexpr uint8_t USER = 0x60;
    static constexpr uint8_t GATE = 0x8e;

    IdtEntry() = default;

    IdtEntry(uintptr_t handler, uint8_t ist, uint8_t attributes)
        : _offsetLow(handler & 0xffff),
          _codeSegment((handler >> 16) & 0xffff),
          _ist(ist),
          _attributes(attributes),
          _offsetMid((handler >> 16) & 0xffff),
          _offsetHigh(handler >> 32),
          _zero(0) {}
};

struct [[gnu::packed]] Idt {
    static constexpr int LEN = 256;

    Karm::Array<IdtEntry, LEN> entries{};
};

extern "C" void _idtLoad(void const *ptr);

struct [[gnu::packed]] IdtDesc {
    uint16_t _limit;
    uint64_t _base;

    IdtDesc(Idt const &base)
        : _limit{sizeof(Idt) - 1}, _base{reinterpret_cast<uintptr_t>(&base)} {}

    void load() const { _idtLoad(this); }
};

} // namespace x86_64
