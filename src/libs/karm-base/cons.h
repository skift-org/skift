#pragma once

#include "std.h"

namespace Karm {

template <typename TCar, typename TCdr = TCar>
struct Cons {
    using Car = TCar;
    using Cdr = TCdr;

    Car car;
    Cdr cdr;

    auto operator<=>(Cons const &) const
        requires Meta::Comparable<Car> and Meta::Comparable<Cdr>
    = default;

    constexpr auto const &get(bool cond) const {
        return cond ? cdr : car;
    }

    constexpr void visit(auto f) {
        f(car);
        f(cdr);
    }

    constexpr void visit(auto f) const {
        f(car);
        f(cdr);
    }
};

template <typename T>
using Pair = Cons<T, T>;

} // namespace Karm
