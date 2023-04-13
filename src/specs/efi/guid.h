#pragma once

#include <karm-base/ordr.h>

namespace Efi {

struct Guid {
    u32 a{};
    u16 b{};
    u16 c{};
    u8 d[8]{};

    Ordr cmp(Guid const &other) const {
        return ::cmp(a, other.a) |
               ::cmp(b, other.b) |
               ::cmp(c, other.c) |
               ::cmp(d, other.d);
    }
};

} // namespace Efi
