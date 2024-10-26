#pragma once

#include "error.h"
#include "union.h"

namespace Karm {

template <typename T = None>
struct Ok {
    T inner;

    template <typename... Args>
    always_inline constexpr Ok(Args &&...args)
        : inner(std::forward<Args>(args)...) {}

    always_inline explicit operator bool() const {
        return true;
    }

    always_inline auto operator<=>(Ok const &) const
        requires Meta::Comparable<T>
    = default;

    T take() {
        return std::move(inner);
    }
};

template <typename... Args>
Ok(Args &&...) -> Ok<Meta::RemoveConstVolatileRef<Args>...>;

template <typename V = None, typename E = Error>
struct [[nodiscard]] Res {
    using Inner = Union<Ok<V>, E>;

    Inner _inner;

    always_inline constexpr Res(Ok<V> const &ok)
        : _inner(ok) {}

    always_inline constexpr Res(Ok<V> &&ok)
        : _inner(std::move(ok)) {}

    template <typename U>
    always_inline constexpr Res(Ok<U> ok)
        : _inner(Ok<V>{ok.inner}) {}

    always_inline constexpr Res(E err)
        : _inner(err) {}

    always_inline constexpr Res(None)
        : _inner(Error::other("unexpected none")) {}

    template <typename U>
    always_inline constexpr Res(Res<U, E> other)
        : _inner(other._inner) {}

    always_inline constexpr explicit operator bool() const {
        return _inner.template is<Ok<V>>();
    }

    always_inline constexpr bool has() const {
        return _inner.template is<Ok<V>>();
    }

    always_inline constexpr E const &none() const {
        if (not _inner.template is<E>()) [[unlikely]]
            panic("none() called on an ok");

        return _inner.template unwrap<E>();
    }

    always_inline constexpr V &unwrap(char const *msg = "unwraping an error") {
        if (not _inner.template is<Ok<V>>()) [[unlikely]]
            panic(msg);

        return _inner.template unwrap<Ok<V>>().inner;
    }

    always_inline constexpr V const &unwrap(char const *msg = "unwraping an error") const {
        if (not _inner.template is<Ok<V>>()) [[unlikely]]
            panic(msg);

        return _inner.template unwrap<Ok<V>>().inner;
    }

    always_inline constexpr V const &unwrapOr(V const &other) const {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>().inner;
        return other;
    }

    always_inline constexpr V unwrapOrDefault(V other) const {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>().inner;
        return other;
    }

    always_inline constexpr V unwrapOrElse(auto f) const {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>().inner;
        return f();
    }

    always_inline constexpr V take(char const *msg = "take() called on an error") {
        if (not _inner.template is<Ok<V>>()) [[unlikely]]
            panic(msg);

        return _inner.template unwrap<Ok<V>>().take();
    }

    template <typename U>
    always_inline constexpr Res<V, U> mapErr(auto f) {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>();
        return f(_inner.template unwrap<E>());
    }

    template <typename U>
    always_inline constexpr Res<V, U> mapErr() {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>();
        return U{};
    }

    template <typename U>
    always_inline constexpr Res<U, E> map(auto f) {
        if (_inner.template is<Ok<V>>())
            return Ok(f(_inner.template unwrap<Ok<V>>().inner));
        return _inner.template unwrap<E>();
    }

    template <typename U>
    always_inline constexpr Res<U, E> map() {
        if (_inner.template is<Ok<V>>())
            return Ok(_inner.template unwrap<Ok<V>>().inner);
        return _inner.template unwrap<E>();
    }

    always_inline auto operator<=>(Res const &) const
        requires Meta::Comparable<Inner>
    = default;

    always_inline auto operator==(bool b) const {
        return has() == b;
    }
};

static_assert(Tryable<Res<isize, Error>>);

} // namespace Karm
