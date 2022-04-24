#pragma once

#include <karm-base/std.h>
#include <karm-meta/traits.h>

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

    constexpr T *operator->() { return _ptr; }
    constexpr T &operator*() { return *_ptr; }

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
