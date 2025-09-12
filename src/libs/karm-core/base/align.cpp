export module Karm.Core:base.align;

import :base.opt;

namespace Karm {

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 8
// ex: 7 with align = 8 -> 0
export constexpr usize alignDown(usize addr, usize align) {
    return addr & ~(align - 1);
}

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
export constexpr usize alignUp(usize addr, usize align) {
    return (addr + align - 1) & ~(align - 1);
}

export constexpr bool isAlign(usize addr, usize align) {
    return alignDown(addr, align) == addr;
}

export constexpr Opt<bool> ensureAlign(usize addr, usize align) {
    if (not isAlign(addr, align))
        return NONE;
    return true;
}

} // namespace Karm
