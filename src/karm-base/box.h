#pragma once

#include <karm-debug/panic.h>
#include <karm-meta/traits.h>

#include "_prelude.h"

#include "opt.h"
#include "std.h"

namespace Karm::Base {

template <typename T>
struct Box {
    T *_ptr = nullptr;

    constexpr Box() = delete;
    constexpr Box(T *ptr) : _ptr(ptr) {}
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

    constexpr auto operator=(Box const &) -> Box & = delete;

    template <Meta::Derive<T> U>
    constexpr auto operator=(Box &&other) -> Box & {
        delete _ptr;
        _ptr = other._ptr;
        other._ptr = nullptr;
        return *this;
    }

    constexpr auto operator->() -> T * {
        if (!_ptr)
            Debug::panic("Deferencing moved from Box<T>");

        return _ptr;
    }
    constexpr auto operator*() -> T & {
        if (!_ptr)
            Debug::panic("Deferencing moved from Box<T>");

        return *_ptr;
    }
};

template <typename T>
using OptBox = Opt<Box<T>>;

template <typename T, typename... Args>
constexpr static auto make_box(Args... args) -> Box<T> {
    return {new T(std::forward<Args>(args)...)};
}

} // namespace Karm::Base
