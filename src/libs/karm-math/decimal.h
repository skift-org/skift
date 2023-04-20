#pragma once

#include <karm-base/cons.h>
#include <karm-fmt/fmt.h>

#include "funcs.h"

namespace Karm::Math {

struct Decimal {
    isize _value;
    u8 _decimals;

    static Decimal from(f64 value) {
        Decimal res = {};
        i8 sign = +1;

        if (value < 0) {
            sign = -1;
            value = -value;
        }

        u8 curr = 0;
        while (::pow(10.0, curr) <= value)
            curr += 1;
        curr -= 1;

        f64 epsilon = 1e-6;
        while (value >= epsilon or curr >= 0) {
            res._value *= 10;
            i8 digit = (u64)(value * ::pow(0.1, (f64)curr)) % 10;
            res._value += digit;
            value -= digit * ::pow(10.0, (f64)curr);

            if (curr < 0)
                res._decimals += 1;

            curr -= 1;

            if (res._decimals > 6)
                break;
        }

        res._value *= sign;
        return res;
    }

    Decimal()
        : _value(0), _decimals(0) {}

    Decimal(isize value)
        : _value(value), _decimals(0) {}

    Decimal(isize value, u8 decimals)
        : _value(value), _decimals(value == 0 ? 0 : decimals) {}

    static Cons<Decimal, Decimal> align(Decimal car, Decimal cdr) {
        bool carIsLess = car._decimals < cdr._decimals;
        u8 decimals = carIsLess ? cdr._decimals : car._decimals;

        Decimal carAligned = {(isize)::pow(10, decimals - car._decimals) * car._value, decimals};
        Decimal cdrAligned = {(isize)::pow(10, decimals - cdr._decimals) * cdr._value, decimals};

        return {carAligned, cdrAligned};
    }

    Decimal operator-(void) const {
        return {-_value, _decimals};
    }

    Decimal operator+(Decimal const &rhs) {
        auto [car, cdr] = align(*this, rhs);
        return {car._value + cdr._value, car._decimals};
    }
    Decimal operator-(Decimal const &rhs) {
        return *this + -rhs;
    }

    Decimal operator*(Decimal const &rhs) {
        auto [car, cdr] = align(*this, rhs);
        return {car._value * cdr._value, (u8)(_decimals + rhs._decimals)};
    }
    Decimal operator/(Decimal const &rhs) {
        return from(cast<f64>() / rhs.cast<f64>());
    }

    Decimal sqrt() const {
        return from(::sqrt(cast<f64>()));
    }

    Decimal invert() const {
        return Decimal{1} / *this;
    }

    template <typename T>
    T cast() const {
        return (T)_value / ::pow<T>(10, _decimals);
    }
};

} // namespace Karm::Math

template <>
struct Karm::Fmt::Formatter<Karm::Math::Decimal> {
    Res<usize> format(Io::TextWriter &writer, Karm::Math::Decimal dec) {
        return Fmt::format(writer, "{}", dec.cast<isize>());
    }
};
