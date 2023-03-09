#pragma once

#include <karm-base/align.h>
#include <karm-base/error.h>
#include <karm-base/range.h>
#include <karm-base/ref.h>
#include <karm-base/std.h>

namespace Hal {

inline constexpr usize PAGE_SIZE = 0x1000;
inline constexpr usize UPPER_HALF = 0xffff800000000000;

inline usize pageAlignDown(usize addr) {
    return alignDown(addr, PAGE_SIZE);
}

inline usize pageAlignUp(usize addr) {
    return alignUp(addr, PAGE_SIZE);
}

inline bool isPageAlign(usize addr) {
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
    T map(T addr) { return (T)((usize)addr + UPPER_HALF); }

    template <typename T>
    T unmap(T addr) { return (T)((usize)addr - UPPER_HALF); }
};

template <typename Owner, typename Range>
struct Mem {
    Owner *_owner;
    Range _range;

    Mem(Range range)
        : _owner(nullptr),
          _range(range) {}

    Mem(Owner &owner, Range range)
        : _owner(&owner),
          _range(range) {}

    ~Mem() {
        if (_owner)
            _owner->free(_range).unwrap("Failed to free memory");
    }

    Mem(const Mem &) = delete;

    Mem(Mem &&other)
        : _owner(std::exchange(other._owner, nullptr)),
          _range(std::move(other._range)) {
    }

    Mem &operator=(const Mem &) = delete;

    Mem &operator=(Mem &&other) {
        _owner = std::exchange(other._owner, nullptr);
        _range = std::move(other._range);
        return *this;
    }

    Range range() const {
        return _range;
    }
};

} // namespace Hal
