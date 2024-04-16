#pragma once

#include <compare>

namespace Karm {

template <typename T, typename TAG>
struct Distinct {
    T _value;

    constexpr Distinct() = default;

    constexpr explicit Distinct(T value) : _value(value) {}

    constexpr T value() const { return _value; }

    constexpr bool operator==(Distinct<T, TAG> const &other) const = default;

    constexpr auto operator<=>(Distinct<T, TAG> const &other) const = default;

    constexpr Distinct<T, TAG> operator+(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value + other._value);
    }

    constexpr Distinct<T, TAG> operator-(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value - other._value);
    }

    constexpr Distinct<T, TAG> operator*(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value * other._value);
    }

    constexpr Distinct<T, TAG> operator/(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value / other._value);
    }

    constexpr Distinct<T, TAG> operator%(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value % other._value);
    }

    constexpr Distinct<T, TAG> operator&(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value & other._value);
    }

    constexpr Distinct<T, TAG> operator|(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value | other._value);
    }

    constexpr Distinct<T, TAG> operator^(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value ^ other._value);
    }

    constexpr Distinct<T, TAG> operator<<(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value << other._value);
    }

    constexpr Distinct<T, TAG> operator>>(Distinct<T, TAG> other) const {
        return Distinct<T, TAG>(_value >> other._value);
    }

    constexpr Distinct<T, TAG> &operator+=(Distinct<T, TAG> other) {
        _value += other._value;
        return *this;
    }

    constexpr Distinct<T, TAG> &operator-=(Distinct<T, TAG> other) {
        _value -= other._value;
        return *this;
    }

    constexpr Distinct<T, TAG> &operator*=(Distinct<T, TAG> other) {
        _value *= other._value;
        return *this;
    }

    constexpr Distinct<T, TAG> &operator/=(Distinct<T, TAG> other) {
        _value /= other._value;
        return *this;
    }

    constexpr Distinct<T, TAG> &operator%=(Distinct<T, TAG> other) {
        _value %= other._value;
        return *this;
    }

    constexpr Distinct<T, TAG> &operator&=(Distinct<T, TAG> other) {
        _value &= other._value;
        return *this;
    }

    constexpr Distinct<T, TAG> &operator|=(Distinct<T, TAG> other) {
        _value |= other._value;
        return *this;
    }

    constexpr Distinct<T, TAG> &operator^=(Distinct<T, TAG> other) {
        _value ^= other._value;
        return *this;
    }

    constexpr Distinct<T, TAG> &operator<<=(Distinct<T, TAG> other) {
        _value <<= other._value;
        return *this;
    }

    constexpr Distinct<T, TAG> &operator>>=(Distinct<T, TAG> other) {
        _value >>= other._value;
        return *this;
    }
};

static_assert(sizeof(Distinct<int, struct _Tag>) == sizeof(int));
static_assert(alignof(Distinct<int, struct _Tag>) == alignof(int));

} // namespace Karm
