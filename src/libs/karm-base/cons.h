#pragma once

#include "ordr.h"
#include "std.h"

namespace Karm {

template <typename TCar, typename TCdr = TCar>
struct Cons {
    using Car = TCar;
    using Cdr = TCdr;

    Car car;
    Cdr cdr;

    constexpr Ordr cmp(Cons const &other) const {
        return cmp(car, other.car) |
               cmp(cdr, other.cdr);
    }

    constexpr auto get(bool cdr) const {
        return cdr ? this->cdr : this->car;
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
