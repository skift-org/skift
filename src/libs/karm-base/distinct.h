#pragma once

namespace Karm {

template <typename T, typename TAG>
struct Distinct {
    T _value;

    constexpr Distinct() = default;

    constexpr Distinct(T value) : _value(value) {}

    constexpr T value() const { return _value; }

    constexpr bool operator==(Distinct<T, TAG> other) const {
        return _value == other._value;
    }

    constexpr bool operator!=(Distinct<T, TAG> other) const {
        return _value != other._value;
    }

    constexpr bool operator<(Distinct<T, TAG> other) const {
        return _value < other._value;
    }

    constexpr bool operator>(Distinct<T, TAG> other) const {
        return _value > other._value;
    }

    constexpr bool operator<=(Distinct<T, TAG> other) const {
        return _value <= other._value;
    }

    constexpr bool operator>=(Distinct<T, TAG> other) const {
        return _value >= other._value;
    }

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

} // namespace Karm
