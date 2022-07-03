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
        return ::cmp(a, other.a) |
               ::cmp(b, other.b) |
               ::cmp(c, other.c) |
               ::cmp(d, other.d);
    }
};

}; // namespace Karm
