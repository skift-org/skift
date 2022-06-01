#pragma once

#include "_prelude.h"

#include "ordr.h"

namespace Karm {

template <typename TCar, typename TCdr>
struct Cons {
    using Car = TCar;
    using Cdr = TCdr;

    Car car{};
    Cdr cdr{};

    Ordr cmp(Cons const &other) const {
        return Op::cmp(car, other.car) | Op::cmp(cdr, other.cdr);
    }
};

template <typename T>
using Pair = Cons<T, T>;

} // namespace Karm
