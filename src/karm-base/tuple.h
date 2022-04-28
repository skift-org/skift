#pragma once

#include "_prelude.h"

#include "cons.h"
#include "opt.h"

namespace Karm::Base {

template <typename... Ts>
struct Tuple;

template <>
struct Tuple<> {
    static constexpr size_t size = 0;

    constexpr auto len() -> size_t { return 0; }

    constexpr void visit(auto) {}
};

template <typename Car>
struct Tuple<Car> {
    Car car;
    None cdr;

    constexpr auto len() -> size_t {
        return 1;
    }

    constexpr void visit(auto f) {
        f(car);
    }

    constexpr auto cons() -> Cons<Car, None> {
        return {car, None{}};
    }
};

template <typename Car, typename... Cdr>
struct Tuple<Car, Cdr...> {
    Car car;
    Tuple<Cdr...> cdr;

    constexpr auto len() -> size_t {
        return 1 + cdr.len();
    }

    constexpr void visit(auto f) {
        f(car);
        cdr.visit(f);
    }

    constexpr auto cons() -> Cons<Car, None> {
        return {car, cdr.cons()};
    }
};

} // namespace Karm::Base
