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
};

template <typename V = None, typename E = Error>
struct [[nodiscard]] Res {
    Var<Ok<V>, E> inner;

    ALWAYS_INLINE constexpr Res(Ok<V> ok)
        : inner(std::move(ok)) {}

    template <typename U>
    ALWAYS_INLINE constexpr Res(Ok<U> ok)
        : inner(Ok<V>{ok.inner}) {}

    ALWAYS_INLINE constexpr Res(E err)
        : inner(err) {}

    ALWAYS_INLINE constexpr Res(None)
        : inner(E{}) {}

    template <typename U>
    ALWAYS_INLINE constexpr Res(Res<U, E> other)
        : inner(other.inner) {}

    ALWAYS_INLINE constexpr explicit operator bool() const {
        return inner.template is<Ok<V>>();
    }

    ALWAYS_INLINE constexpr bool has() const {
        return inner.template is<Ok<V>>();
    }

    ALWAYS_INLINE constexpr E none() const {
        if (not inner.template is<E>()) {
            panic("none() called on an ok");
        }
        return inner.template unwrap<E>();
    }

    ALWAYS_INLINE constexpr V &unwrap(char const *msg = "unwraping an error") {
        if (not inner.template is<Ok<V>>()) {
            panic(msg);
        }
        return inner.template unwrap<Ok<V>>().inner;
    }

    ALWAYS_INLINE constexpr V const &unwrap(char const *msg = "unwraping an error") const {
        if (not inner.template is<Ok<V>>()) {
            panic(msg);
        }
        return inner.template unwrap<Ok<V>>().inner;
    }

    ALWAYS_INLINE constexpr V take() {
        if (not inner.template is<Ok<V>>()) {
            panic("take() called on an error");
        }
        return std::move(inner.template unwrap<Ok<V>>().inner);
    }

    template <typename U>
    ALWAYS_INLINE constexpr Res<V, U> mapErr(auto f) {
        if (inner.template is<Ok<V>>()) {
            return inner.template unwrap<Ok<V>>();
        }
        return f(inner.template unwrap<E>());
    }

    template <typename U>
    ALWAYS_INLINE constexpr Res<V, U> mapErr() {
        if (inner.template is<Ok<V>>()) {
            return inner.template unwrap<Ok<V>>();
        }
        return U{};
    }

    template <typename U>
    ALWAYS_INLINE constexpr Res<U, E> mapValue(auto f) {
        if (inner.template is<Ok<V>>()) {
            return f(inner.template unwrap<Ok<V>>().inner);
        }
        return inner.template unwrap<E>();
    }

    template <typename U>
    ALWAYS_INLINE constexpr Res<U, E> mapValue() {
        if (inner.template is<Ok<V>>()) {
            return inner.template unwrap<Ok<V>>().inner;
        }
        return inner.template unwrap<E>();
    }
};

static_assert(Tryable<Res<isize, Error>>);

} // namespace Karm
