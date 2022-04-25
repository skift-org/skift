#pragma once

#include <karm-debug/panic.h>
#include <karm-meta/traits.h>

#include "std.h"

namespace Karm::Base
{

template <typename T>
struct Box
{
    T *_ptr = nullptr;

    constexpr Box() = delete;
    constexpr Box(T *ptr) : _ptr(ptr) {}
    constexpr Box(Box const &) = delete;

    template <Meta::Derive<T> U>
    constexpr Box(Box<U> &&other)
    {
        delete _ptr;
        _ptr = other._ptr;
        other._ptr = nullptr;
    }

    constexpr ~Box()
    {
        if (_ptr)
            delete _ptr;
    }

    constexpr Box &operator=(Box const &) = delete;

    template <Meta::Derive<T> U>
    constexpr Box &operator=(Box &&other)
    {
        delete _ptr;
        _ptr = other._ptr;
        other._ptr = nullptr;
        return *this;
    }

    constexpr T *operator->()
    {
        if (!_ptr)
            Debug::panic("Deferencing moved from Box<T>");

        return _ptr;
    }
    constexpr T &operator*()
    {
        if (!_ptr)
            Debug::panic("Deferencing moved from Box<T>");

        return *_ptr;
    }

    template <typename... Args>
    constexpr static Box<T> make(Args... args)
    {
        return {new T(std::forward<Args>(args)...)};
    }

    template <Meta::Derive<T> U, typename... Args>
    constexpr static Box<T> make(Args... args)
    {
        return {new U(std::forward<Args>(args)...)};
    }
};

} // namespace Karm::Base
