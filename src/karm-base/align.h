#pragma once

#include "std.h"

namespace Karm {

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 8
// ex: 7 with align = 8 -> 0
template <typename T>
T align_down(T addr, size_t align) {
    return addr & ~(align - 1);
}

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
template <typename T>
T align_up(T addr, size_t align) {
    return (addr + align - 1) & ~(align - 1);
}

} // namespace Karm
