#pragma once

#include <karm-base/checked.h>
#include <karm-io/fmt.h>

#include "funcs.h"

namespace Karm::Math {

template <typename T>
struct Frac;

template <Meta::SignedIntegral T, usize _F>
struct Fixed {
    static constexpr bool _FIXED = true;

    static constexpr usize _FRAC = _F;
    static constexpr usize _DENO = 1 << _F;
    static constexpr T _MASK = _DENO - 1;

    static constexpr T _MIN = Limits<T>::MIN;
    static constexpr T _MAX = Limits<T>::MAX;

    using Raw = T;

    T _val;

    static constexpr Fixed fromRaw(T val) {
        Fixed f;
        f._val = val;
        return f;
    }

    template <Meta::SignedIntegral I>
    static constexpr Fixed fromInt(I val) {
        if (val < _MIN or val > _MAX)
            return fromRaw(val < _MIN ? _MIN : _MAX);
        return fromRaw(static_cast<T>(val << _FRAC));
    }

    template <Meta::UnsignedIntegral U>
    static constexpr Fixed fromUint(U val) {
        if (val > _MAX)
            return fromRaw(_MAX);
        return fromRaw(static_cast<T>(val << _FRAC));
    }

    template <Meta::Float F>
    static constexpr Fixed fromFloatNearest(F val) {
        T raw = 0;
        if (not isNan(val))
            raw = clampTo<T>(val * _DENO);
        return fromRaw(raw);
    }

    template <Meta::Float F>
    static constexpr Fixed fromFloatFloor(F val) {
        T raw = 0;
        if (not isnan(val))
            raw = clampTo<T>(floor(val * _DENO));
        return fromRaw(raw);
    }

    template <Meta::Float F>
    static constexpr Fixed fromFloatCeil(F val) {
        T raw = 0;
        if (not isnan(val))
            raw = clampTo<T>(ceil(val * _DENO));
        return fromRaw(raw);
    }

    constexpr Fixed() : _val(0) {}

    template <Meta::Float F>
    explicit constexpr Fixed(F from) : _val(fromFloatNearest(from)._val) {}

    template <Meta::SignedIntegral I>
    explicit constexpr Fixed(I from) : _val(fromInt<I>(from)._val) {}

    template <Meta::UnsignedIntegral U>
    explicit constexpr Fixed(U from) : _val(fromUint<U>(from)._val) {}

    constexpr T raw() const {
        return _val;
    }

    template <typename U>
    constexpr U cast() const {
        if constexpr (Meta::Integral<U>)
            return static_cast<U>(_val >> _FRAC);
        else
            return static_cast<U>(_val) / _DENO;
    }

    template <typename U>
        requires Meta::Float<U> or Meta::Integral<U>
    explicit constexpr operator U() const {
        return cast<U>();
    }

    constexpr Fixed& operator++() {
        _val = saturatingAdd<T>(_val, _DENO);
        return *this;
    }

    constexpr Fixed operator++(int) {
        Fixed f = *this;
        ++*this;
        return f;
    }

    constexpr Fixed& operator--() {
        _val = saturatingSub<T>(_val, _DENO);
        return *this;
    }

    constexpr Fixed operator--(int) {
        Fixed f = *this;
        --*this;
        return f;
    }

    constexpr Fixed operator+() const {
        return *this;
    }

    constexpr Fixed operator-() const {
        return fromRaw(-_val);
    }

    constexpr Fixed operator+(Fixed const& rhs) const {
        return fromRaw(saturatingAdd<T>(_val, rhs._val));
    }

    constexpr Fixed operator-(Fixed const& rhs) const {
        return fromRaw(saturatingSub<T>(_val, rhs._val));
    }

    constexpr Fixed operator*(Fixed const& rhs) const {
        isize val = _val;
        val *= rhs._val;

        T ival = clampTo<T>(val >> _FRAC);

        if (val & (1u << (_FRAC - 1))) {
            if (val & (_MASK >> 1u))
                ival = saturatingAdd<T>(ival, 1);
            else
                ival = saturatingAdd<T>(ival, ival & 1);
        }

        return fromRaw(ival);
    }

    constexpr Fixed loosyDiv(Fixed const& rhs) const {
        if (rhs._val == 0)
            panic("division by zero");

        isize val = _val;
        val <<= _FRAC;
        val /= rhs._val;
        return fromRaw(clampTo<T>(val));
    }

    constexpr Frac<Fixed> operator/(Fixed const& rhs) const;

    constexpr Fixed operator/(Frac<Fixed> const& rhs) const;

    constexpr Fixed& operator+=(Fixed const& rhs) {
        return *this = *this + rhs;
    }

    constexpr Fixed& operator-=(Fixed const& rhs) {
        return *this = *this - rhs;
    }

    constexpr Fixed& operator*=(Fixed const& rhs) {
        return *this = *this * rhs;
    }

    constexpr Frac<Fixed>& operator/=(Fixed const& rhs) {
        return *this = *this / rhs;
    }

    constexpr bool operator==(Fixed const& rhs) const = default;

    constexpr std::strong_ordering operator<=>(Fixed const& rhs) const = default;
};

template <typename T>
struct Frac {
    T _num;
    T _deno;

    Frac(T num, T deno = 1)
        : _num(num), _deno(deno) {}

    template <Meta::SignedIntegral I>
    Frac(I num, I deno = 1)
        : _num(num), _deno(deno) {}

    constexpr operator T() const {
        return _num.loosyDiv(_deno);
    }
};

template <Meta::SignedIntegral T, usize F>
constexpr Frac<Fixed<T, F>> Fixed<T, F>::operator/(Fixed<T, F> const& rhs) const {
    return Frac<Fixed<T, F>>{fromRaw(_val), rhs};
}

template <Meta::SignedIntegral T, usize F>
constexpr Fixed<T, F> Fixed<T, F>::operator/(Frac<Fixed<T, F>> const& rhs) const {
    return fromRaw(saturatingDiv(_val, rhs._num) * rhs._deno);
}

using i24f8 = Fixed<i32, 8>;
using i16f16 = Fixed<i32, 16>;
using i8f24 = Fixed<i32, 24>;

// MARK: Functions -------------------------------------------------------------

template <Meta::SignedIntegral T, usize _F>
constexpr Fixed<T, _F> abs(Fixed<T, _F> const& val) {
    return val < Fixed<T, _F>{0} ? -val : val;
}

} // namespace Karm::Math

template <Meta::SignedIntegral T, usize F>
struct Karm::Limits<Math::Fixed<T, F>> {
    static constexpr Math::Fixed<T, F> MIN = Math::Fixed<T, F>::fromRaw(Limits<T>::MIN);
    static constexpr Math::Fixed<T, F> MAX = Math::Fixed<T, F>::fromRaw(Limits<T>::MAX);
    static constexpr Math::Fixed<T, F> EPSILON = Math::Fixed<T, F>::fromRaw(1);
    static constexpr bool SIGNED = false;
};

template <Meta::SignedIntegral T, usize F>
struct Karm::Io::Formatter<Math::Fixed<T, F>> {
    Res<> format(Io::TextWriter& writer, Math::Fixed<T, F> const& val) {
        return Io::format(writer, "{}", val.template cast<f64>());
    }
};

template <typename T>
struct Karm::Io::Formatter<Math::Frac<T>> {
    Res<> format(Io::TextWriter& writer, Math::Frac<T> const& val) {
        return Io::format(writer, "{}/{}", val._num, val._deno);
    }
};
