#pragma once

#include <karm-base/cons.h>
#include <karm-fmt/fmt.h>

#include "funcs.h"

namespace Karm::Math {

struct Decimal {
    int64_t _value;
    uint8_t _decimals;

    static Decimal from(double value) {
        Decimal res = {};
        int sign = +1;

        if (value < 0) {
            sign = -1;
            value = -value;
        }

        uint8_t curr = 0;
        while (Math::pow(10.0, curr) <= value)
            curr += 1;
        curr -= 1;

        double epsilon = 1e-6;
        while (value >= epsilon or curr >= 0) {
            res._value *= 10;
            int8_t digit = (uint64_t)(value * Math::pow(0.1, (double)curr)) % 10;
            res._value += digit;
            value -= digit * Math::pow(10.0, (double)curr);

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

    Decimal(int64_t value)
        : _value(value), _decimals(0) {}

    Decimal(int64_t value, uint8_t decimals)
        : _value(value), _decimals(value == 0 ? 0 : decimals) {}

    static Cons<Decimal, Decimal> align(Decimal car, Decimal cdr) {
        bool carIsLess = car._decimals < cdr._decimals;
        uint8_t decimals = carIsLess ? cdr._decimals : car._decimals;

        Decimal carAligned = {Math::pow<int64_t>(10, decimals - car._decimals) * car._value, decimals};
        Decimal cdrAligned = {Math::pow<int64_t>(10, decimals - cdr._decimals) * cdr._value, decimals};

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
        return {car._value * cdr._value, (uint8_t)(_decimals + rhs._decimals)};
    }
    Decimal operator/(Decimal const &rhs) {
        return from(cast<double>() / rhs.cast<double>());
    }

    Decimal sqrt() const {
        return from(::sqrt(cast<double>()));
    }

    Decimal invert() const {
        return Decimal{1} / *this;
    }

    template <typename T>
    T cast() const {
        return (T)_value / Math::pow<T>(10, _decimals);
    }
};

} // namespace Karm::Math

template <>
struct Karm::Fmt::Formatter<Karm::Math::Decimal> {
    Result<size_t> format(Io::_TextWriter &writer, Karm::Math::Decimal dec) {
        return Fmt::format(writer, "{}", dec.cast<int>());
    }
};
