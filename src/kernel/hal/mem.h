#pragma once

#include <karm-base/align.h>
#include <karm-base/error.h>
#include <karm-base/std.h>

namespace Hal {

inline constexpr size_t PAGE_SIZE = 0x1000;
inline constexpr size_t UPPER_HALF = 0xffff800000000000;

inline size_t pageAlignDown(size_t addr) {
    return alignDown(addr, PAGE_SIZE);
}

inline size_t pageAlignUp(size_t addr) {
    return alignUp(addr, PAGE_SIZE);
}

inline bool isPageAlign(size_t addr) {
    return isAlign(addr, PAGE_SIZE);
}

struct IdentityMapper {
    template <typename T>
    T map(T addr) { return addr; }
    template <typename T>
    T unmap(T addr) { return addr; }
};

struct UpperHalfMapper {
    template <typename T>
    T map(T addr) { return (T)((uintptr_t)addr + UPPER_HALF); }

    template <typename T>
    T unmap(T addr) { return (T)((uintptr_t)addr - UPPER_HALF); }
};

} // namespace Hal
