module;

#include <karm-core/macros.h>

export module Karm.Core:base.box;

import :meta.traits;
import :base.opt;

namespace Karm {

export template <typename T>
struct Box {
    T* _ptr{};

    constexpr Box() = delete;

    constexpr Box(Move, T* ptr)
        : _ptr(ptr) {}

    constexpr Box(T const& v)
        : _ptr(new T(v)) {}

    constexpr Box(T&& v)
        : _ptr(new T(std::move(v))) {}

    constexpr Box(Box const& other)
        : _ptr(new T(*other._ptr)) {}

    template <Meta::Derive<T> U>
    constexpr Box(Box<U>&& other)
        : _ptr(std::exchange(other._ptr, nullptr)) {}

    constexpr ~Box() {
        if (_ptr)
            delete _ptr;
        _ptr = nullptr;
    }

    constexpr Box& operator=(Box const& other) {
        *this = Box(other);
        return *this;
    }

    template <Meta::Derive<T> U>
    constexpr Box& operator=(Box<U>&& other) {
        std::swap(_ptr, other._ptr);
        return *this;
    }

    template <typename U>
        requires Meta::Equatable<T, U>
    constexpr bool operator==(U const& other) const {
        return unwrap() == other;
    }

    template <typename U>
        requires Meta::Comparable<T, U>
    constexpr auto operator<=>(U const& other) const {
        return unwrap() <=> other;
    }

    template <typename U>
        requires Meta::Equatable<T, U>
    constexpr bool operator==(Box<U> const& other) const {
        if (_ptr == other._ptr)
            return true;
        return unwrap() == other.unwrap();
    }

    template <typename U>
        requires Meta::Comparable<T, U>
    constexpr auto operator<=>(Box<U> const& other) const {
        return unwrap() <=> other.unwrap();
    }

    constexpr T* operator->() lifetimebound {
        return &unwrap();
    }

    constexpr T& operator*() lifetimebound {
        return unwrap();
    }

    constexpr T const* operator->() const lifetimebound {
        return &unwrap();
    }

    constexpr T const& operator*() const lifetimebound {
        return unwrap();
    }

    constexpr operator T&() lifetimebound {
        return unwrap();
    }

    constexpr operator T const&() const lifetimebound {
        return unwrap();
    }

    constexpr T const& unwrap() const lifetimebound {
        if (not _ptr) [[unlikely]]
            panic("deferencing moved from Box<T>");
        return *_ptr;
    }

    constexpr T& unwrap() lifetimebound {
        if (not _ptr) [[unlikely]]
            panic("deferencing moved from Box<T>");
        return *_ptr;
    }

    constexpr u64 hash() const {
        return hash(unwrap());
    }
};

export template <typename T>
struct Niche<Box<T>> {
    struct Content {
        char const* ptr;

        always_inline constexpr Content() : ptr(NICHE_PTR) {}

        always_inline constexpr bool has() const {
            return ptr != NICHE_PTR;
        }
    };
};

export template <typename T, typename... Args>
constexpr Box<T> makeBox(Args... args) {
    return {MOVE, new T(std::forward<Args>(args)...)};
}

} // namespace Karm
