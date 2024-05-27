#pragma once

#include <compare>

#include "macros.h"

namespace Karm {

template <typename T, typename TAG>
struct Distinct {
    T _value;

    always_inline constexpr Distinct() = default;

    always_inline constexpr explicit Distinct(T value) : _value(value) {}

    always_inline constexpr T value() const { return _value; }

    always_inline constexpr bool operator==(Distinct<T, TAG> const &other) const = default;

    always_inline constexpr auto operator<=>(Distinct<T, TAG> const &other) const = default;

    always_inline constexpr Distinct<T, TAG> operator+(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value + other._value);
    }

    always_inline constexpr Distinct<T, TAG> operator-(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value - other._value);
    }

    always_inline constexpr Distinct<T, TAG> operator*(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value * other._value);
    }

    always_inline constexpr Distinct<T, TAG> operator/(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value / other._value);
    }

    always_inline constexpr Distinct<T, TAG> operator%(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value % other._value);
    }

    always_inline constexpr Distinct<T, TAG> operator&(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value & other._value);
    }

    always_inline constexpr Distinct<T, TAG> operator|(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value | other._value);
    }

    always_inline constexpr Distinct<T, TAG> operator^(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value ^ other._value);
    }

    always_inline constexpr Distinct<T, TAG> operator<<(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value << other._value);
    }

    always_inline constexpr Distinct<T, TAG> operator>>(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value >> other._value);
    }

    always_inline constexpr Distinct<T, TAG> &operator+=(Distinct<T, TAG> other) {
        _value += other._value;
        return *this;
    }

    always_inline constexpr Distinct<T, TAG> &operator-=(Distinct<T, TAG> other) {
        _value -= other._value;
        return *this;
    }

    always_inline constexpr Distinct<T, TAG> &operator*=(Distinct<T, TAG> other) {
        _value *= other._value;
        return *this;
    }

    always_inline constexpr Distinct<T, TAG> &operator/=(Distinct<T, TAG> other) {
        _value /= other._value;
        return *this;
    }

    always_inline constexpr Distinct<T, TAG> &operator%=(Distinct<T, TAG> other) {
        _value %= other._value;
        return *this;
    }

    always_inline constexpr Distinct<T, TAG> &operator&=(Distinct<T, TAG> other) {
        _value &= other._value;
        return *this;
    }

    always_inline constexpr Distinct<T, TAG> &operator|=(Distinct<T, TAG> other) {
        _value |= other._value;
        return *this;
    }

    always_inline constexpr Distinct<T, TAG> &operator^=(Distinct<T, TAG> other) {
        _value ^= other._value;
        return *this;
    }

    always_inline constexpr Distinct<T, TAG> &operator<<=(Distinct<T, TAG> other) {
        _value <<= other._value;
        return *this;
    }

    always_inline constexpr Distinct<T, TAG> &operator>>=(Distinct<T, TAG> other) {
        _value >>= other._value;
        return *this;
    }
};

static_assert(sizeof(Distinct<int, struct _Tag>) == sizeof(int));
static_assert(alignof(Distinct<int, struct _Tag>) == alignof(int));

} // namespace Karm
