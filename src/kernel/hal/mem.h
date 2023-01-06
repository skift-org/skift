#pragma once

#include <karm-base/align.h>
#include <karm-base/error.h>
#include <karm-base/range.h>
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

template <typename Owner, typename Range>
struct Mem {
    Owner &_owner;
    bool _owned;
    Range _range;

    Mem(Owner &owner, Range range)
        : _owner(owner),
          _owned(true),
          _range(range) {}

    ~Mem() {
        if (_owned)
            _owner.free(_range).unwrap("Failed to free memory");
    }

    Mem(const Mem &) = delete;

    Mem(Mem &&other)
        : _owner(other._owner),
          _owned(other._owned),
          _range(other._range) {
        other._owned = false;
    }

    Mem &operator=(const Mem &) = delete;

    Mem &operator=(Mem &&other) {
        *this = std::move(other);
        return *this;
    }

    USizeRange range() const {
        return _range;
    }
};

} // namespace Hal
