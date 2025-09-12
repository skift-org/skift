#pragma once

import Karm.Core;

namespace Karm::Math {

// MARK: Unsigned Big Integer --------------------------------------------------
// aka natural number

struct UBig;

void _add(UBig& lhs, usize rhs);

void _add(UBig& lhs, UBig const& rhs);

enum struct [[nodiscard]] SubResult {
    OK,
    UNDERFLOW,
};

SubResult _sub(UBig& lhs, usize rhs);

SubResult _sub(UBig& lhs, UBig const& rhs);

void _shl(UBig& lhs, usize bits);

void _shr(UBig& lhs, usize bits);

void _binNot(UBig& lhs);

void _binOr(UBig& lhs, UBig const& rhs);

void _binAnd(UBig& lhs, UBig const& rhs);

void _binXor(UBig& lhs, UBig const& rhs);

void _mul(UBig& lhs, UBig const& rhs);

void _div(UBig const& numerator, UBig const& denominator, UBig& quotient, UBig& remainder);

void _gcd(UBig const& lhs, UBig const& rhs, UBig& gcd);

void _pow(UBig const& base, UBig const& exp, UBig& res);

struct UBig {
    Vec<usize> _value;

    UBig() = default;

    explicit UBig(usize value) {
        if (value != 0)
            _value.pushBack(value);
    }

    UBig& operator=(usize value) {
        _value.clear();
        if (value != 0)
            _value.pushBack(value);
        return *this;
    }

    usize _len() const {
        return _value.len();
    }

    void _trim() {
        while (_value.len() >= 1 and last(_value) == 0)
            _value.popBack();
    }

    void clear() {
        _value.clear();
    }

    void _setBit(usize bit) {
        if (bit >= _value.len() * Limits<usize>::BITS)
            _value.resize(bit / Limits<usize>::BITS + 1);
        _value[bit / Limits<usize>::BITS] |= 1 << (bit % Limits<usize>::BITS);
    }

    bool _getBit(usize bit) const {
        return bit < _value.len() * Limits<usize>::BITS and
               (_value[bit / Limits<usize>::BITS] & (1 << (bit % Limits<usize>::BITS))) != 0;
    }

    UBig operator~() {
        UBig res = *this;
        _binNot(res);
        return res;
    }

    UBig operator|(UBig const& rhs) const {
        UBig res = *this;
        _binOr(res, rhs);
        return res;
    }

    UBig operator&(UBig const& rhs) const {
        UBig res = *this;
        _binAnd(res, rhs);
        return res;
    }

    UBig operator^(UBig const& rhs) const {
        UBig res = *this;
        _binXor(res, rhs);
        return res;
    }

    UBig operator<<(usize bits) const {
        UBig res = *this;
        _shl(res, bits);
        return res;
    }

    UBig operator>>(usize bits) const {
        UBig res = *this;
        _shr(res, bits);
        return res;
    }

    UBig operator+(UBig const& rhs) const {
        UBig res = *this;
        _add(res, rhs);
        res._trim();
        return res;
    }

    UBig operator-(UBig const& rhs) const {
        UBig res = *this;
        // FIXME: we ignore underflow here
        (void)_sub(res, rhs);
        res._trim();
        return res;
    }

    UBig operator*(UBig const& rhs) const {
        UBig res = *this;
        _mul(res, rhs);
        res._trim();
        return res;
    }

    UBig operator/(UBig const& rhs) const {
        UBig res, remainder;
        _div(*this, rhs, res, remainder);
        res._trim();
        return res;
    }

    UBig operator%(UBig const& rhs) const {
        UBig quotient, res;
        _div(*this, rhs, quotient, res);
        res._trim();
        return res;
    }

    UBig& operator|=(UBig const& rhs) {
        _binOr(*this, rhs);
        return *this;
    }

    UBig& operator&=(UBig const& rhs) {
        _binAnd(*this, rhs);
        return *this;
    }

    UBig& operator^=(UBig const& rhs) {
        _binXor(*this, rhs);
        return *this;
    }

    UBig& operator<<=(usize bits) {
        _shl(*this, bits);
        return *this;
    }

    UBig& operator>>=(usize bits) {
        _shr(*this, bits);
        return *this;
    }

    UBig& operator++() {
        _add(*this, 1);
        _trim();
        return *this;
    }

    UBig& operator--() {
        // FIXME: we ignore underflow here
        (void)_sub(*this, 1);
        _trim();
        return *this;
    }

    UBig operator++(int) {
        UBig res = *this;
        _add(*this, 1);
        _trim();
        return res;
    }

    UBig operator--(int) {
        UBig res = *this;
        // FIXME: we ignore underflow here
        (void)_sub(*this, 1);
        _trim();
        return res;
    }

    UBig& operator+=(UBig const& rhs) {
        _add(*this, rhs);
        _trim();
        return *this;
    }

    UBig& operator-=(UBig const& rhs) {
        // FIXME: we ignore underflow here
        (void)_sub(*this, rhs);
        _trim();
        return *this;
    }

    UBig& operator*=(UBig const& rhs) {
        _mul(*this, rhs);
        _trim();
        return *this;
    }

    UBig& operator/=(UBig const& rhs) {
        UBig quotient, remainder;
        _div(*this, rhs, quotient, remainder);
        *this = quotient;
        _trim();
        return *this;
    }

    UBig& operator%=(UBig const& rhs) {
        UBig quotient, remainder;
        _div(*this, rhs, quotient, remainder);
        *this = remainder;
        _trim();
        return *this;
    }

    explicit operator f64() const {
        f64 res = 0;
        for (usize i = _len(); i-- > 0;) {
            res *= Math::pow<f64>(2.0, Limits<usize>::BITS);
            res += _value[i];
        }
        return res;
    }

    std::strong_ordering operator<=>(UBig const& rhs) const {
        if (_len() != rhs._len())
            return _len() <=> rhs._len();
        for (usize i = _len(); i-- > 0;) {
            if (_value[i] != rhs._value[i])
                return _value[i] <=> rhs._value[i];
        }
        return std::strong_ordering::equal;
    }

    std::strong_ordering operator<=>(usize rhs) const {
        if (_len() == 0)
            return 0 <=> rhs;

        if (_len() != 1)
            return std::strong_ordering::greater;

        return _value[0] <=> rhs;
    }

    bool operator==(UBig const& rhs) const {
        return _len() == rhs._len() and _value == rhs._value;
    }

    bool operator==(usize rhs) const {
        return _len() == 1 and _value[0] == rhs;
    }
};

// MARK: Signed Big Integer ----------------------------------------------------
// aka integer number

struct IBig;

void _add(IBig& lhs, usize rhs);

void _add(IBig& lhs, IBig const& rhs);

void _sub(IBig& lhs, usize rhs);

void _sub(IBig& lhs, IBig const& rhs);

void _mul(IBig& lhs, IBig const& rhs);

void _div(IBig const& numerator, IBig const& denominator, IBig& quotient, IBig& remainder);

void _pow(IBig const& base, UBig const& exp, IBig& res);

enum struct Sign {
    POSITIVE,
    NEGATIVE
};

static constexpr Sign signOf(usize) {
    return Sign::POSITIVE;
}

static constexpr Sign signOf(isize value) {
    return value < 0 ? Sign::NEGATIVE : Sign::POSITIVE;
}

struct IBig {
    UBig _value;
    Sign _sign;

    IBig() = default;

    explicit IBig(usize value, Sign sign = Sign::POSITIVE)
        : _value(value), _sign(value == 0 ? Sign::POSITIVE : sign) {}

    IBig& operator=(usize value) {
        _value = value;
        _sign = Sign::POSITIVE;
        return *this;
    }

    explicit IBig(isize value)
        : _value(static_cast<usize>(value < 0 ? -value : value)),
          _sign(value < 0 ? Sign::NEGATIVE : Sign::POSITIVE) {}

    IBig& operator=(isize value) {
        _value = static_cast<usize>(value < 0 ? -value : value);
        _sign = value < 0 ? Sign::NEGATIVE : Sign::POSITIVE;
        return *this;
    }

    explicit IBig(UBig const& big, Sign sign = Sign::POSITIVE)
        : _value(big), _sign(big == 0 ? Sign::POSITIVE : sign) {}

    bool negative() const {
        return _sign == Sign::NEGATIVE;
    }

    bool positive() const {
        return _sign == Sign::POSITIVE;
    }

    Sign sign() const {
        return _sign;
    }

    UBig& value() {
        return _value;
    }

    UBig const& value() const {
        return _value;
    }

    usize _len() const {
        return _value._len();
    }

    void _trim() {
        _value._trim();
        if (_value == 0)
            _sign = Sign::POSITIVE;
    }

    void clear() {
        _value.clear();
        _sign = Sign::POSITIVE;
    }

    IBig operator-() const {
        return IBig{
            _value,
            _sign == Sign::POSITIVE
                ? Sign::NEGATIVE
                : Sign::POSITIVE,
        };
    }

    IBig operator+(IBig const& rhs) const {
        IBig res = *this;
        _add(res, rhs);
        res._trim();
        return res;
    }

    IBig operator-(IBig const& rhs) const {
        IBig res = *this;
        _sub(res, rhs);
        res._trim();
        return res;
    }

    IBig operator*(IBig const& rhs) const {
        IBig res = *this;
        _mul(res, rhs);
        res._trim();
        return res;
    }

    IBig operator/(IBig const& rhs) const {
        IBig res, remainder;
        _div(*this, rhs, res, remainder);
        res._trim();
        return res;
    }

    IBig operator%(IBig const& rhs) const {
        IBig quotient, res;
        _div(*this, rhs, quotient, res);
        res._trim();
        return res;
    }

    IBig& operator++() {
        _add(*this, 1);
        _trim();
        return *this;
    }

    IBig& operator--() {
        _sub(*this, 1);
        _trim();
        return *this;
    }

    IBig operator++(int) {
        IBig res = *this;
        _add(*this, 1);
        _trim();
        return res;
    }

    IBig operator--(int) {
        IBig res = *this;
        _sub(*this, 1);
        _trim();
        return res;
    }

    IBig& operator+=(IBig const& rhs) {
        _add(*this, rhs);
        _trim();
        return *this;
    }

    IBig& operator-=(IBig const& rhs) {
        _sub(*this, rhs);
        _trim();
        return *this;
    }

    IBig& operator*=(IBig const& rhs) {
        _mul(*this, rhs);
        _trim();
        return *this;
    }

    IBig& operator/=(IBig const& rhs) {
        IBig res, remainder;
        _div(*this, rhs, res, remainder);
        *this = res;

        _trim();
        return *this;
    }

    IBig& operator%=(IBig const& rhs) {
        IBig quotient, res;
        _div(*this, rhs, quotient, res);
        *this = res;

        _trim();
        return *this;
    }

    std::strong_ordering operator<=>(IBig const& rhs) const {
        if (_sign != rhs._sign)
            return _sign == Sign::NEGATIVE
                       ? std::strong_ordering::less
                       : std::strong_ordering::greater;
        if (_sign == Sign::NEGATIVE)
            return _value <=> rhs._value;
        return rhs._value <=> _value;
    }

    std::strong_ordering operator<=>(usize rhs) const {
        if (_sign == Sign::NEGATIVE)
            return std::strong_ordering::less;
        if (_value._len() != 1)
            return std::strong_ordering::greater;
        return _value._value[0] <=> rhs;
    }

    std::strong_ordering operator<=>(isize rhs) const {
        if (_sign != signOf(rhs))
            return _sign == Sign::NEGATIVE
                       ? std::strong_ordering::less
                       : std::strong_ordering::greater;
        if (_sign == Sign::NEGATIVE)
            return _value <=> static_cast<usize>(rhs < 0 ? -rhs : rhs);

        return static_cast<usize>(rhs < 0 ? -rhs : rhs) <=> _value._value[0];
    }

    bool operator==(IBig const& rhs) const {
        return _sign == rhs._sign and _value == rhs._value;
    }

    bool operator==(usize rhs) const {
        return _sign == Sign::POSITIVE and _value == rhs;
    }

    bool operator==(isize rhs) const {
        return _sign == signOf(rhs) and
               _value == static_cast<usize>(rhs < 0 ? -rhs : rhs);
    }
};

// MARK: Big Fractional Number -------------------------------------------------
// aka rational number

struct BigFrac;

void _fromF64(BigFrac& frac, f64 value);

void _add(BigFrac& lhs, BigFrac const& rhs);

void _sub(BigFrac& lhs, BigFrac const& rhs);

void _mul(BigFrac& lhs, BigFrac const& rhs);

void _div(BigFrac& lhs, BigFrac const& rhs);

void _mod(BigFrac& lhs, BigFrac const& rhs);

struct BigFrac {
    IBig _num;
    UBig _den;

    BigFrac() = default;

    void _reduce() {
        UBig gcd;
        _gcd(_num.value(), _den, gcd);
        if (gcd == 1uz)
            return;

        _num.value() /= gcd;
        _den /= gcd;
    }

    void clear() {
        _num.clear();
        _den = 1uz;
    }

    explicit BigFrac(usize value)
        : _num(value), _den(1uz) {}

    BigFrac& operator=(usize value) {
        _num = value;
        _den = 1uz;
        return *this;
    }

    explicit BigFrac(isize value)
        : _num(value), _den(1uz) {}

    BigFrac& operator=(isize value) {
        _num = value;
        _den = 1uz;
        return *this;
    }

    explicit BigFrac(isize num, usize den)
        : _num(num), _den(den) {}

    explicit BigFrac(IBig const& num, UBig const& den)
        : _num(num), _den(den) {}

    explicit BigFrac(f64 value) {
        _fromF64(*this, value);
    }

    BigFrac& operator=(f64 value) {
        _fromF64(*this, value);
        return *this;
    }

    IBig& num() {
        return _num;
    }

    IBig const& num() const {
        return _num;
    }

    UBig& den() {
        return _den;
    }

    UBig const& den() const {
        return _den;
    }

    BigFrac operator-() const {
        return BigFrac{-_num, _den};
    }

    BigFrac operator+(BigFrac const& rhs) const {
        BigFrac res = *this;
        _add(res, rhs);
        res._reduce();
        return res;
    }

    BigFrac operator-(BigFrac const& rhs) const {
        BigFrac res = *this;
        _sub(res, rhs);
        res._reduce();
        return res;
    }

    BigFrac operator*(BigFrac const& rhs) const {
        BigFrac res = *this;
        _mul(res, rhs);
        res._reduce();
        return res;
    }

    BigFrac operator/(BigFrac const& rhs) const {
        BigFrac res = *this;
        _div(res, rhs);
        res._reduce();
        return res;
    }

    BigFrac& operator+=(BigFrac const& rhs) {
        _add(*this, rhs);
        _reduce();
        return *this;
    }

    BigFrac& operator-=(BigFrac const& rhs) {
        _sub(*this, rhs);
        _reduce();
        return *this;
    }

    BigFrac& operator*=(BigFrac const& rhs) {
        _mul(*this, rhs);
        _reduce();
        return *this;
    }

    BigFrac& operator/=(BigFrac const& rhs) {
        _div(*this, rhs);
        _reduce();
        return *this;
    }

    std::strong_ordering operator<=>(BigFrac const& rhs) const {
        auto cmp = *this - rhs;
        if (cmp._num == 0uz)
            return std::strong_ordering::equal;

        if (cmp._num.negative())
            return std::strong_ordering::less;

        return std::strong_ordering::greater;
    }

    bool operator==(BigFrac const& rhs) const {
        return _num == rhs._num and _den == rhs._den;
    }
};

} // namespace Karm::Math

static inline Karm::Math::UBig operator""_ubig(unsigned long long value) {
    return Karm::Math::UBig{static_cast<Karm::usize>(value)};
}

static inline Karm::Math::IBig operator""_ibig(unsigned long long value) {
    return Karm::Math::IBig{static_cast<Karm::usize>(value)};
}

static inline Karm::Math::BigFrac operator""_bigfrac(unsigned long long value) {
    return Karm::Math::BigFrac{static_cast<Karm::usize>(value)};
}

static inline Karm::Math::BigFrac operator""_bigfrac(long double value) {
    return Karm::Math::BigFrac{static_cast<Karm::f64>(value)};
}
