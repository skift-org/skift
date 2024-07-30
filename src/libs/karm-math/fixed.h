#pragma once

#include <karm-base/checked.h>
#include <karm-io/fmt.h>

namespace Karm::Math {

template <typename T>
struct _Frac;

template <Meta::SignedIntegral T, usize _F>
struct _Fixed {
    static constexpr bool _FIXED = true;

    static constexpr usize FRAC = _F;
    static constexpr usize DENO = 1 << _F;
    static constexpr T MASK = DENO - 1;

    static constexpr T MIN = Limits<T>::MIN;
    static constexpr T MAX = Limits<T>::MAX;

    using Raw = T;

    T _val;

    static constexpr _Fixed fromRaw(T val) {
        _Fixed f;
        f._val = val;
        return f;
    }

    template <Meta::SignedIntegral I>
    static constexpr _Fixed fromInt(I val) {
        if (val < MIN or val > MAX)
            return fromRaw(val < MIN ? MIN : MAX);
        return fromRaw(static_cast<T>(val << FRAC));
    }

    template <Meta::UnsignedIntegral U>
    static constexpr _Fixed fromUint(U val) {
        if (val > MAX)
            return fromRaw(MAX);
        return fromRaw(static_cast<T>(val << FRAC));
    }

    template <Meta::Float F>
    static constexpr _Fixed fromFloatNearest(F val) {
        T raw = 0;
        if (not isnan(val))
            raw = clampTo<T>(val * DENO);
        return fromRaw(raw);
    }

    template <Meta::Float F>
    static constexpr _Fixed fromFloatFloor(F val) {
        T raw = 0;
        if (not isnan(val))
            raw = clampTo<T>(floor(val * DENO));
        return fromRaw(raw);
    }

    template <Meta::Float F>
    static constexpr _Fixed fromFloatCeil(F val) {
        T raw = 0;
        if (not isnan(val))
            raw = clampTo<T>(ceil(val * DENO));
        return fromRaw(raw);
    }

    constexpr _Fixed() : _val(0) {}

    template <Meta::Float F>
    explicit constexpr _Fixed(F from) : _val(fromFloatNearest(from)._val) {}

    template <Meta::SignedIntegral I>
    explicit constexpr _Fixed(I from) : _val(fromInt<I>(from)._val) {}

    template <Meta::UnsignedIntegral U>
    explicit constexpr _Fixed(U from) : _val(fromUint<U>(from)._val) {}

    constexpr T raw() const {
        return _val;
    }

    template <Meta::SignedIntegral I>
    constexpr I toInt() const {
        return static_cast<I>(_val >> FRAC);
    }

    template <Meta::UnsignedIntegral U>
    constexpr U toUint() const {
        return static_cast<U>(_val >> FRAC);
    }

    template <Meta::Float F>
    constexpr F toFloat() const {
        return static_cast<F>(_val) / DENO;
    }

    template <Meta::SignedIntegral I>
    explicit constexpr operator I() const {
        return toInt<I>();
    }

    template <Meta::UnsignedIntegral U>
    explicit constexpr operator U() const {
        return toUint<U>();
    }

    template <Meta::Float F>
    explicit constexpr operator F() const {
        return toFloat<F>();
    }

    constexpr _Fixed &operator++() {
        _val = saturatingAdd<T>(_val, DENO);
        return *this;
    }

    constexpr _Fixed operator++(int) {
        _Fixed f = *this;
        ++*this;
        return f;
    }

    constexpr _Fixed &operator--() {
        _val = saturatingSub<T>(_val, DENO);
        return *this;
    }

    constexpr _Fixed operator--(int) {
        _Fixed f = *this;
        --*this;
        return f;
    }

    constexpr _Fixed operator+() const {
        return *this;
    }

    constexpr _Fixed operator-() const {
        return fromRaw(-_val);
    }

    constexpr _Fixed operator+(_Fixed const &rhs) const {
        return fromRaw(saturatingAdd<T>(_val, rhs._val));
    }

    constexpr _Fixed operator-(_Fixed const &rhs) const {
        return fromRaw(saturatingSub<T>(_val, rhs._val));
    }

    constexpr _Fixed operator*(_Fixed const &rhs) const {
        isize val = _val;
        val *= rhs._val;

        T ival = clampTo<T>(val >> FRAC);

        if (val & (1u << (FRAC - 1))) {
            if (val & (MASK >> 1u))
                ival = saturatingAdd<T>(ival, 1);
            else
                ival = saturatingAdd<T>(ival, ival & 1);
        }

        return fromRaw(ival);
    }

    constexpr _Fixed loosyDiv(_Fixed const &rhs) const {
        isize val = _val;
        val <<= FRAC;
        val /= rhs._val;
        return fromRaw(clampTo<T>(val));
    }

    constexpr _Frac<_Fixed> operator/(_Fixed const &rhs) const;

    constexpr _Fixed operator/(_Frac<_Fixed> const &rhs) const;

    constexpr _Fixed &operator+=(_Fixed const &rhs) {
        return *this = *this + rhs;
    }

    constexpr _Fixed &operator-=(_Fixed const &rhs) {
        return *this = *this - rhs;
    }

    constexpr _Fixed &operator*=(_Fixed const &rhs) {
        return *this = *this * rhs;
    }

    constexpr _Frac<_Fixed> &operator/=(_Fixed const &rhs) {
        return *this = *this / rhs;
    }

    constexpr bool operator==(_Fixed const &rhs) const = default;

    constexpr std::strong_ordering operator<=>(_Fixed const &rhs) const = default;
};

template <typename T>
struct _Frac {
    T _num;
    T _deno;

    _Frac(T num, T deno = 1)
        : _num(num), _deno(deno) {}

    template <Meta::SignedIntegral I>
    _Frac(I num, I deno = 1)
        : _num(num), _deno(deno) {}

    constexpr operator T() const {
        return _num.loosyDiv(_deno);
    }
};

template <Meta::SignedIntegral T, usize F>
constexpr _Frac<_Fixed<T, F>> _Fixed<T, F>::operator/(_Fixed<T, F> const &rhs) const {
    return _Frac<_Fixed<T, F>>{fromRaw(_val), rhs};
}

template <Meta::SignedIntegral T, usize F>
constexpr _Fixed<T, F> _Fixed<T, F>::operator/(_Frac<_Fixed<T, F>> const &rhs) const {
    return fromRaw(saturatingDiv(_val, rhs._num) * rhs._deno);
}

using i24f8 = _Fixed<i32, 8>;
using i16f16 = _Fixed<i32, 16>;
using i8f24 = _Fixed<i32, 24>;

} // namespace Karm::Math

template <typename T, usize F>
struct Karm::Limits<Math::_Fixed<T, F>> {
    static constexpr Math::_Fixed<T, F> MIN = Math::_Fixed<T, F>::fromRaw(0);
    static constexpr Math::_Fixed<T, F> MAX = Math::_Fixed<T, F>::fromRaw(Limits<T>::MAX);
    static constexpr Math::_Fixed<T, F> EPSILON = Math::_Fixed<T, F>::fromRaw(1);
    static constexpr bool SIGNED = false;
};

template <Meta::SignedIntegral T, usize F>
struct Karm::Io::Formatter<Math::_Fixed<T, F>> {
    Res<usize> format(Io::TextWriter &writer, Math::_Fixed<T, F> const &val) {
        return Io::format(writer, "{}", val.template toFloat<f64>());
    }
};

template <typename T>
struct Karm::Io::Formatter<Math::_Frac<T>> {
    Res<usize> format(Io::TextWriter &writer, Math::_Frac<T> const &val) {
        return Io::format(writer, "{}/{}", val._num, val._deno);
    }
};
