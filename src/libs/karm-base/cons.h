#pragma once

#include "_prelude.h"

#include "ordr.h"

namespace Karm {

template <typename TCar, typename TCdr = TCar>
struct Cons {
    using Car = TCar;
    using Cdr = TCdr;

    Car car{};
    Cdr cdr{};

    Ordr cmp(Cons const &other) const {
        return cmp(car, other.car) |
               cmp(cdr, other.cdr);
    }

    auto get(bool cdr) const {
        return cdr ? this->cdr : this->car;
    }
};

template <typename T>
using Pair = Cons<T, T>;

} // namespace Karm
