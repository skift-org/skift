#pragma once

#include <karm-meta/traits.h>

#include "opt.h"
#include "std.h"

namespace Karm {

template <typename T>
struct Box {
    T *_ptr{};

    constexpr Box() = delete;

    constexpr Box(Move, T *ptr)
        : _ptr(ptr) {}

    constexpr Box(T const &v)
        : _ptr(new T(v)) {}

    constexpr Box(T &&v)
        : _ptr(new T(std::move(v))) {}

    constexpr Box(Box const &other)
        : _ptr(new T(*other._ptr)) {}

    template <Meta::Derive<T> U>
    constexpr Box(Box<U> &&other)
        : _ptr(std::exchange(other._ptr, nullptr)) {}

    constexpr ~Box() {
        if (_ptr)
            delete _ptr;
        _ptr = nullptr;
    }

    constexpr Box &operator=(Box const &other) {
        *this = Box(other);
        return *this;
    }

    template <Meta::Derive<T> U>
    constexpr Box &operator=(Box<U> &&other) {
        std::swap(_ptr, other._ptr);
        return *this;
    }

    constexpr T *operator->() {
        if (not _ptr) [[unlikely]]
            panic("deferencing moved from Box<T>");

        return _ptr;
    }

    constexpr T &operator*() {
        if (not _ptr) [[unlikely]]
            panic("deferencing moved from Box<T>");

        return *_ptr;
    }

    constexpr T const *operator->() const {
        if (not _ptr) [[unlikely]]
            panic("deferencing moved from Box<T>");

        return _ptr;
    }

    constexpr T const &operator*() const {
        if (not _ptr) [[unlikely]]
            panic("deferencing moved from Box<T>");

        return *_ptr;
    }
};

template <typename T, typename... Args>
constexpr static Box<T> makeBox(Args... args) {
    return {MOVE, new T(std::forward<Args>(args)...)};
}

} // namespace Karm
