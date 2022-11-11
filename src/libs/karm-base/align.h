#pragma once

#include "error.h"
#include "std.h"

namespace Karm {

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 8
// ex: 7 with align = 8 -> 0
inline constexpr size_t alignDown(size_t addr, size_t align) {
    return addr & ~(align - 1);
}

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
inline constexpr size_t alignUp(size_t addr, size_t align) {
    return (addr + align - 1) & ~(align - 1);
}

inline constexpr bool isAlign(size_t addr, size_t align) {
    return alignDown(addr, align) == addr;
}

inline constexpr Error ensureAlign(size_t addr, size_t align) {
    return isAlign(addr, align) ? OK : Error{Error::INVALID_DATA, "not aligned"};
}

} // namespace Karm
