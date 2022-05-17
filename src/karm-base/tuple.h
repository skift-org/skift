#pragma once

#include "_prelude.h"

#include "cons.h"
#include "opt.h"

namespace Karm {

template <typename... Ts>
struct Tuple;

template <>
struct Tuple<> {
    static constexpr size_t size = 0;

    constexpr Tuple() {}

    constexpr size_t len() { return 0; }

    constexpr void visit(auto) {}
};

template <typename Car>
struct Tuple<Car> {
    Car car;
    None cdr;

    constexpr Tuple(Car &&car) : car(car) {}

    constexpr size_t len() {
        return 1;
    }

    constexpr void visit(auto f) {
        f(car);
    }

    constexpr Cons<Car, None> cons() {
        return {car, None{}};
    }
};

template <typename Car, typename... Cdr>
struct Tuple<Car, Cdr...> {
    Car car;
    Tuple<Cdr...> cdr;

    constexpr Tuple(Car &&car, Cdr &&...cdr) : car(car), cdr(std::forward<Cdr>(cdr)...) {
    }

    constexpr size_t len() {
        return 1 + cdr.len();
    }

    constexpr void visit(auto f) {
        f(car);
        cdr.visit(f);
    }

    constexpr Cons<Car, Tuple<Cdr...>> cons() {
        return {car, cdr.cons()};
    }
};

} // namespace Karm
