#pragma once

#include "_prelude.h"

#include "error.h"
#include "std.h"
#include "var.h"

namespace Karm {

template <typename T = None>
struct Ok {
    T inner;

    constexpr Ok()
        : inner() {}

    constexpr Ok(T value)
        : inner(std::move(value)) {}
};


template <typename V = None, typename E = Error>
struct [[nodiscard]] Res {
    Var<Ok<V>, E> inner;

    constexpr Res(Ok<V> ok)
        : inner(std::move(ok)) {}

    template <typename U>
    constexpr Res(Ok<U> ok)
        : inner(Ok<V>{ok.inner}) {}

    constexpr Res(E err)
        : inner(err) {}

    constexpr Res(None)
        : inner(E{}) {}

    template <typename U>
    constexpr Res(Res<U, E> other)
        : inner(other.inner) {}

    constexpr explicit operator bool() const {
        return inner.template is<Ok<V>>();
    }

    constexpr E none() const {
        if (not inner.template is<E>()) {
            panic("none() called on an ok");
        }
        return inner.template unwrap<E>();
    }

    constexpr V &unwrap(char const *msg = "unwraping an error") {
        if (not inner.template is<Ok<V>>()) {
            panic(msg);
        }
        return inner.template unwrap<Ok<V>>().inner;
    }

    constexpr V const &unwrap(char const *msg = "unwraping an error") const {
        if (not inner.template is<Ok<V>>()) {
            panic(msg);
        }
        return inner.template unwrap<Ok<V>>().inner;
    }

    constexpr V take() {
        if (not inner.template is<Ok<V>>()) {
            panic("take() called on an error");
        }
        return std::move(inner.template unwrap<Ok<V>>().inner);
    }
};

static_assert(Tryable<Res<int, Error>>);

} // namespace Karm
