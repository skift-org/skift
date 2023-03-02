#pragma once

#include "ordr.h"
#include "std.h"

namespace Karm {

struct Uuid {
    u32 a{};
    u16 b{};
    u16 c{};
    u8 d[8]{};

    Ordr cmp(Uuid const &other) const {
        return ::cmp(a, other.a) |
               ::cmp(b, other.b) |
               ::cmp(c, other.c) |
               ::cmp(d, other.d);
    }
};

}; // namespace Karm
