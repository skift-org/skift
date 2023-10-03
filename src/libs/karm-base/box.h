#pragma once

#include <karm-meta/traits.h>

#include "opt.h"
#include "std.h"

namespace Karm {

template <typename T>
struct Box {
    T *_ptr{};

    constexpr Box() = delete;

    constexpr Box(T *ptr)
        : _ptr(ptr) {}

    constexpr Box(Box const &) = delete;

    template <Meta::Derive<T> U>
    constexpr Box(Box<U> &&other) {
        delete _ptr;
        _ptr = other._ptr;
        other._ptr = nullptr;
    }

    constexpr ~Box() {
        if (_ptr)
            delete _ptr;
    }

    constexpr Box &operator=(Box const &) = delete;

    template <Meta::Derive<T> U>
    constexpr Box &operator=(Box<U> &&other) {
        delete _ptr;
        _ptr = other._ptr;
        other._ptr = nullptr;
        return *this;
    }

    constexpr T *operator->() {
        if (not _ptr)
            panic("Deferencing moved from Box<T>");

        return _ptr;
    }

    constexpr T &operator*() {
        if (not _ptr)
            panic("Deferencing moved from Box<T>");

        return *_ptr;
    }

    constexpr T const *operator->() const {
        if (not _ptr)
            panic("Deferencing moved from Box<T>");

        return _ptr;
    }

    constexpr T const &operator*() const {
        if (not _ptr)
            panic("Deferencing moved from Box<T>");

        return *_ptr;
    }
};

template <typename T, typename... Args>
constexpr static Box<T> makeBox(Args... args) {
    return {new T(std::forward<Args>(args)...)};
}

} // namespace Karm
