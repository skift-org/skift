#pragma once

#include "error.h"
#include "std.h"
#include "var.h"

namespace Karm {

template <typename T = None>
struct Ok {
    T inner;

    ALWAYS_INLINE constexpr Ok()
        : inner() {}

    ALWAYS_INLINE constexpr Ok(T value)
        : inner(std::move(value)) {}

    operator bool() const = delete;

    ALWAYS_INLINE auto operator<=>(Ok const &) const
        requires Meta::Comparable<T>
    = default;
};

template <typename V = None, typename E = Error>
struct [[nodiscard]] Res {
    using Inner = Var<Ok<V>, E>;

    Inner _inner;

    ALWAYS_INLINE constexpr Res(Ok<V> ok)
        : _inner(std::move(ok)) {}

    template <typename U>
    ALWAYS_INLINE constexpr Res(Ok<U> ok)
        : _inner(Ok<V>{ok.inner}) {}

    ALWAYS_INLINE constexpr Res(E err)
        : _inner(err) {}

    ALWAYS_INLINE constexpr Res(None)
        : _inner(E{}) {}

    template <typename U>
    ALWAYS_INLINE constexpr Res(Res<U, E> other)
        : _inner(other._inner) {}

    ALWAYS_INLINE constexpr explicit operator bool() const {
        return _inner.template is<Ok<V>>();
    }

    ALWAYS_INLINE constexpr bool has() const {
        return _inner.template is<Ok<V>>();
    }

    ALWAYS_INLINE constexpr E none() const {
        if (not _inner.template is<E>()) {
            panic("none() called on an ok");
        }
        return _inner.template unwrap<E>();
    }

    ALWAYS_INLINE constexpr V &unwrap(char const *msg = "unwraping an error") {
        if (not _inner.template is<Ok<V>>()) {
            panic(msg);
        }
        return _inner.template unwrap<Ok<V>>().inner;
    }

    ALWAYS_INLINE constexpr V const &unwrap(char const *msg = "unwraping an error") const {
        if (not _inner.template is<Ok<V>>()) {
            panic(msg);
        }
        return _inner.template unwrap<Ok<V>>().inner;
    }

    ALWAYS_INLINE constexpr V take() {
        if (not _inner.template is<Ok<V>>()) {
            panic("take() called on an error");
        }
        return std::move(_inner.template unwrap<Ok<V>>().inner);
    }

    template <typename U>
    ALWAYS_INLINE constexpr Res<V, U> mapErr(auto f) {
        if (_inner.template is<Ok<V>>()) {
            return _inner.template unwrap<Ok<V>>();
        }
        return f(_inner.template unwrap<E>());
    }

    template <typename U>
    ALWAYS_INLINE constexpr Res<V, U> mapErr() {
        if (_inner.template is<Ok<V>>()) {
            return _inner.template unwrap<Ok<V>>();
        }
        return U{};
    }

    template <typename U>
    ALWAYS_INLINE constexpr Res<U, E> mapValue(auto f) {
        if (_inner.template is<Ok<V>>()) {
            return f(_inner.template unwrap<Ok<V>>().inner);
        }
        return _inner.template unwrap<E>();
    }

    template <typename U>
    ALWAYS_INLINE constexpr Res<U, E> mapValue() {
        if (_inner.template is<Ok<V>>()) {
            return _inner.template unwrap<Ok<V>>().inner;
        }
        return _inner.template unwrap<E>();
    }

    ALWAYS_INLINE auto operator<=>(Res const &) const
        requires Meta::Comparable<Inner>
    = default;
};

static_assert(Tryable<Res<isize, Error>>);

} // namespace Karm
