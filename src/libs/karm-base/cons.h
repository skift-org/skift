#pragma once

#include "base.h"

namespace Karm {

template <typename TCar, typename TCdr = TCar>
struct Cons {
    using Car = TCar;
    using Cdr = TCdr;

    Car car;
    Cdr cdr;

    always_inline constexpr auto const &get(bool cond) const {
        return cond ? cdr : car;
    }

    always_inline constexpr void visit(auto f) {
        f(car);
        f(cdr);
    }

    always_inline constexpr void visit(auto f) const {
        f(car);
        f(cdr);
    }

    template <typename TCar2, typename TCdr2 = TCar2>
    always_inline constexpr Cons<TCar2, TCdr2> cast() const {
        return {static_cast<TCar2>(car), static_cast<TCdr2>(cdr)};
    }

    template <typename U>
    always_inline constexpr U into() const {
        return U{car, cdr};
    }

    always_inline bool operator==(Cons const &) const
        requires Meta::Equatable<Car, Cdr>
    = default;

    always_inline auto operator<=>(Cons const &) const
        requires Meta::Comparable<Car> and Meta::Comparable<Cdr>
    = default;
};

template <typename TCar, typename TCdr>
Cons(TCar, TCdr) -> Cons<TCar, TCdr>;

template <typename T>
using Pair = Cons<T, T>;

} // namespace Karm
