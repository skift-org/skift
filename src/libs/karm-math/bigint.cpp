#include "bigint.h"

#include "funcs.h"

namespace Karm::Math {

// MARK: Unsigned Big Integer --------------------------------------------------

void _add(UBig& lhs, usize rhs) {
    auto lhsLen = lhs._len();

    usize carry = 0;
    usize curr = 0;
    if (willAddOverflow(rhs, lhs._value[0]))
        curr = 1;

    usize res = rhs + lhs._value[0];
    if (willAddOverflow(res, carry))
        curr = 1;

    res += carry;
    carry = curr;
    lhs._value[0] = res;

    for (usize i = 1; carry and lhsLen > i; i++) {
        usize curr = 0;
        if (willAddOverflow(lhs._value[i], carry))
            curr = 1;
        lhs._value[i] += carry;
        carry = curr;
    }

    if (carry)
        lhs._value.pushBack(carry);
}

void _add(UBig& lhs, UBig const& rhs) {
    auto lhsLen = lhs._len();
    auto rhsLen = rhs._len();

    usize carry = 0;
    for (usize i = 0; i < rhsLen; ++i) {
        usize curr = 0;
        if (willAddOverflow(rhs._value[i], lhs._value[i]))
            curr = 1;

        usize res = rhs._value[i] + lhs._value[i];
        if (willAddOverflow(res, carry))
            curr = 1;

        res += carry;
        carry = curr;
        lhs._value[i] = res;
    }

    while (carry and lhsLen > rhsLen) {
        usize curr = 0;
        if (willAddOverflow(lhs._value[rhsLen], carry))
            curr = 1;

        lhs._value[rhsLen] += carry;
        carry = curr;
        rhsLen++;
    }

    if (carry) {
        lhs._value.pushBack(carry);
    }
}

SubResult _sub(UBig& lhs, usize rhs) {
    u8 borrow = 0;
    for (usize i = 0; i < lhs._len(); i++) {
        usize rhsV = i < 1 ? rhs : 0;

        usize lhsV = lhs._value[i];

        // apply the borrow
        if (borrow) {
            if (lhsV == 0) {
                lhsV = Limits<usize>::MAX;
                borrow = 1;
            } else {
                lhsV--;
                borrow = 0;
            }
        }

        // do we need to borrow?
        if (lhsV < rhsV) {
            rhsV -= lhsV;
            lhsV = (Limits<usize>::MAX - rhsV) + 1;
            borrow = 1;
        }

        lhsV -= rhsV;
        lhs._value[i] = lhsV;
    }

    if (borrow)
        return SubResult::UNDERFLOW;

    return SubResult::OK;
}

SubResult _sub(UBig& lhs, UBig const& rhs) {
    u8 borrow = 0;
    for (usize i = 0; i < lhs._len(); i++) {
        usize rhsV = i < rhs._len()
                         ? rhs._value[i]
                         : 0;

        usize lhsV = lhs._value[i];

        // apply the borrow
        if (borrow) {
            if (lhsV == 0) {
                lhsV = Limits<usize>::MAX;
                borrow = 1;
            } else {
                lhsV--;
                borrow = 0;
            }
        }

        // do we need to borrow?
        if (lhsV < rhsV) {
            rhsV -= lhsV;
            lhsV = (Limits<usize>::MAX - rhsV) + 1;
            borrow = 1;
        }

        lhsV -= rhsV;
        lhs._value[i] = lhsV;
    }

    if (borrow)
        return SubResult::UNDERFLOW;

    return SubResult::OK;
}

void _shl(UBig& lhs, usize bits) {
    if (lhs == 0 or bits == 0)
        return;

    usize carry = 0;

    for (usize i = 0; i < lhs._len(); ++i) {
        usize value = lhs._value[i];
        lhs._value[i] = (value << bits) | carry;
        carry = value >> (Limits<usize>::BITS - bits);
    }

    if (carry)
        lhs._value.pushBack(carry);
}

void _shr(UBig& lhs, usize bits) {
    if (lhs == 0 or bits == 0)
        return;

    usize carry = 0;

    for (usize i = lhs._len(); i-- > 0;) {
        usize value = lhs._value[i];
        lhs._value[i] = (value >> bits) | carry;
        carry = value << (Limits<usize>::BITS - bits);
    }
}

void _binNot(UBig& lhs) {
    for (usize i = 0; i < lhs._len(); ++i)
        lhs._value[i] = ~lhs._value[i];
}

void _binOr(UBig& lhs, UBig const& rhs) {
    lhs._value.resize(max(lhs._len(), rhs._len()));

    for (usize i = 0; i < rhs._len(); ++i)
        lhs._value[i] |= rhs._value[i];
}

void _binAnd(UBig& lhs, UBig const& rhs) {
    lhs._value.resize(max(lhs._len(), rhs._len()));

    for (usize i = 0; i < rhs._len(); ++i)
        lhs._value[i] &= rhs._value[i];
}

void _binXor(UBig& lhs, UBig const& rhs) {
    lhs._value.resize(max(lhs._len(), rhs._len()));

    for (usize i = 0; i < rhs._len(); ++i)
        lhs._value[i] ^= rhs._value[i];
}

void _mul(UBig& lhs, UBig const& rhs) {
    UBig shifted = lhs;
    for (usize wi = 0; wi < rhs._len(); wi++) {
        for (usize bi = 0; bi < Limits<usize>::BITS; bi++) {
            auto bit = (rhs._value[wi] >> bi) & 1;
            _shl(shifted, 1);
            if (not bit)
                continue;
            _add(lhs, shifted);
        }
    }
}

void _div(UBig const& numerator, UBig const& denominator, UBig& quotient, UBig& remainder) {
    quotient = 0_ubig;
    remainder = numerator;

    for (int wi = numerator._len() - 1; wi >= 0; wi--) {
        for (int bi = Limits<usize>::BITS - 1; bi >= 0; bi--) {
            usize shift = wi * Limits<usize>::BITS + bi;
            UBig shifted = denominator;
            _shl(shifted, shift);
            auto tmp = remainder;
            if (_sub(tmp, shifted) == SubResult::UNDERFLOW)
                continue;
            remainder = tmp;
            quotient._setBit(shift);
        }
    }
}

void _gcd(UBig const& lhs, UBig const& rhs, UBig& gcd) {
    if (lhs == 0 or rhs == 0) [[unlikely]]
        panic("gcd of zero");

    gcd = lhs;
    UBig b = rhs;

    while (b != 0_ubig) {
        UBig tmp = b;
        b = gcd % b;
        gcd = tmp;
    }
}

void _pow(UBig const& base, UBig const& exp, UBig& res) {
    UBig b{base}, e{exp};
    res = 1_ubig;

    while (not(e < 1_ubig)) {
        if (e._value[0] % 2 == 1)
            _mul(res, b);
        _shr(e, 1);
        _mul(b, b);
    }
}

// MARK: Signed Big Integer ----------------------------------------------------

void _add(IBig& lhs, IBig const& rhs) {
    if (lhs._sign == rhs._sign) {
        _add(lhs._value, rhs._value);
    } else if (lhs < rhs) {
        IBig res = rhs;
        _sub(res, lhs);
        lhs = std::move(res);
    } else {
        _sub(lhs, rhs);
    }
}

void _sub(IBig& lhs, IBig const& rhs) {
    if (lhs._sign != rhs._sign) {
        _add(lhs._value, rhs._value);
    } else if (lhs.positive()) {
        if (lhs._value < rhs._value) {
            IBig res = rhs;
            if (_sub(res._value, lhs._value) == SubResult::UNDERFLOW) [[unlikely]]
                panic("unexpected underflow");

            lhs = std::move(res);
            lhs._sign = Sign::NEGATIVE;
        } else {
            if (_sub(lhs._value, rhs._value) == SubResult::UNDERFLOW) [[unlikely]]
                panic("unexpected underflow");

            lhs._sign = Sign::POSITIVE;
        }
    } else {
        if (lhs._value > rhs._value) {
            if (_sub(lhs._value, rhs._value) == SubResult::UNDERFLOW) [[unlikely]]
                panic("unexpected underflow");

            lhs._sign = Sign::NEGATIVE;
        } else {
            IBig res = rhs;
            if (_sub(res._value, lhs._value) == SubResult::UNDERFLOW) [[unlikely]]
                panic("unexpected underflow");

            lhs = res;
            lhs._sign = Sign::POSITIVE;
        }
    }
}

void _mul(IBig& lhs, IBig const& rhs) {
    _mul(lhs._value, rhs._value);
    lhs._sign = lhs._sign == rhs._sign ? Sign::POSITIVE : Sign::NEGATIVE;
}

void _div(IBig const& numerator, IBig const& denominator, IBig& quotient, IBig& remainder) {
    _div(numerator._value, denominator._value, quotient._value, remainder._value);
    quotient._sign = numerator._sign == denominator._sign ? Sign::POSITIVE : Sign::NEGATIVE;
    remainder._sign = numerator._sign;
}

void _pow(IBig const& base, UBig const& exp, IBig& res) {
    IBig b{base}, e{exp};
    res = 1_ibig;

    while (not(e < 1_ibig)) {
        if (e._value._value[0] % 2 == 1)
            _mul(res, b);
        _shr(e._value, 1);
        _mul(b, b);
    }
}

// MARK: Big Fractional Numbers ------------------------------------------------

void _fromF64(BigFrac& frac, f64 value) {
    IBig const TEN = 10_ibig;

    frac.clear();

    bool neg = false;
    if (value < 0) {
        neg = true;
        value = -value;
    }

    isize currPow = 0;
    while (Math::pow<f64>(10.0, currPow) <= value) {
        currPow += 1;
    }
    currPow -= 1;

    UBig dec = 0_ubig;
    while (value >= __DBL_EPSILON__ or currPow >= 0) {
        frac._num *= TEN;
        usize digit = (u64)(value * Math::pow(0.1, (f64)currPow)) % 10;
        frac._num += IBig{digit};
        value -= digit * Math::pow(10.0, (f64)currPow);
        if (currPow < 0) {
            ++dec;
            _pow(TEN.value(), dec, frac._den);
        }
        currPow -= 1;
    }

    frac._num = neg ? -frac._num : frac._num;
}

void _add(BigFrac& lhs, BigFrac const& rhs) {
    if (rhs._num == 0uz)
        return;

    IBig tmp = rhs._num;
    _mul(tmp.value(), lhs._den);
    _mul(lhs._num.value(), rhs._den);
    _add(lhs._num, tmp);

    _mul(lhs._den, rhs._den);
}

void _sub(BigFrac& lhs, BigFrac const& rhs) {
    _add(lhs, -rhs);
}

void _mul(BigFrac& lhs, BigFrac const& rhs) {
    _mul(lhs._num, rhs._num);
    _mul(lhs._den, rhs._den);
}

void _div(BigFrac& lhs, BigFrac const& rhs) {
    _mul(lhs._num.value(), rhs._den);
    _mul(lhs._den, rhs._num.value());
}

} // namespace Karm::Math
