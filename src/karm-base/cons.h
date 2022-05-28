#pragma once

#include "_prelude.h"

namespace Karm {

template <typename TCar, typename TCdr>
struct Cons {
    using Car = TCar;
    using Cdr = TCdr;

    Car car;
    Cdr cdr;
};

template <typename T>
using Pair = Cons<T, T>;

} // namespace Karm
