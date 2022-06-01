#pragma once

#include "ordr.h"
#include "std.h"

namespace Karm {

struct Uuid {
    uint32_t a{};
    uint16_t b{};
    uint16_t c{};
    uint8_t d[8]{};

    Ordr cmp(Uuid const &other) const {
        return Op::cmp(a, other.a) |
               Op::cmp(b, other.b) |
               Op::cmp(c, other.c) |
               Op::cmp(d, other.d);
    }
};

}; // namespace Karm
