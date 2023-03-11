#pragma once

#include "res.h"
#include "std.h"

namespace Karm {

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 8
// ex: 7 with align = 8 -> 0
inline constexpr usize alignDown(usize addr, usize align) {
    return addr & ~(align - 1);
}

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
inline constexpr usize alignUp(usize addr, usize align) {
    return (addr + align - 1) & ~(align - 1);
}

inline constexpr bool isAlign(usize addr, usize align) {
    return alignDown(addr, align) == addr;
}

static inline Res<> ensureAlign(usize addr, usize align) {
    if (not isAlign(addr, align)) {
        return Error::invalidInput("not aligned");
    }

    return Ok();
}

} // namespace Karm
