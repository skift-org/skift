module;

#include <karm-core/macros.h>

export module Karm.Core:io.aton;

import :math.funcs;
import :base.ctype;
import :io.sscan;

namespace Karm::Io {

export struct AtoxOptions {
    u8 base = 10;
    bool allowExp = true;
};

Opt<u8> _parseDigit(Rune rune, AtoxOptions options = {}) {
    rune = toAsciiLower(rune);
    u8 result = 255;

    if (isAsciiAlpha(rune))
        result = rune - 'a' + 10;
    else if (isAsciiDigit(rune))
        result = rune - '0';

    if (result >= options.base)
        return NONE;

    return result;
}

template <StaticEncoding E>
Opt<u8> _nextDigit(_SScan<E>& s, AtoxOptions options = {}) {
    if (s.ended())
        return NONE;

    auto d = _parseDigit(s.peek(), options);
    if (d)
        s.next();
    return d;
}

export template <StaticEncoding E>
Opt<usize> atou(_SScan<E>& s, AtoxOptions options = {}) {
    bool isNum = false;
    usize result = 0;

    while (not s.ended()) {
        auto maybeDigit = _nextDigit(s, options);
        if (not maybeDigit)
            break;
        isNum = true;
        result = result * options.base + maybeDigit.unwrap();
    }

    if (not isNum)
        return NONE;

    return result;
}

export template <StaticEncoding E>
Opt<isize> atoi(_SScan<E>& s, AtoxOptions options = {}) {
    bool isNeg = false;
    bool isNum = false;
    isize result = 0;

    auto rollback = s.rollbackPoint();

    if (not s.skip('+'))
        isNeg = s.skip('-');

    while (not s.ended()) {
        auto maybeDigit = _nextDigit(s, options);
        if (not maybeDigit)
            break;
        isNum = true;
        result = result * options.base + maybeDigit.unwrap();
    }

    if (not isNum)
        return NONE;

    if (isNeg)
        result = -result;

    rollback.disarm();
    return result;
}

#ifndef __ck_freestanding__

export template <StaticEncoding E>
Opt<f64> atof(_SScan<E>& s, AtoxOptions const& options = {}) {
    Opt<i64> ipart;
    f64 fpart = 0.0;
    i64 exp = 0;
    bool neg = false;
    bool hasFpart = false;

    auto rollback = s.rollbackPoint();

    if (not s.skip('+'))
        neg = s.skip('-');

    if (s.peek(0) != '.' and _parseDigit(s.peek(0), options)) {
        ipart = try$(atoi(s, options));
    }

    if (s.skip('.')) {
        f64 multiplier = (1.0 / options.base);
        while (not s.ended()) {
            auto maybeDigit = _nextDigit(s, options);
            if (not maybeDigit)
                break;
            hasFpart = true;
            fpart += maybeDigit.unwrap() * multiplier;
            multiplier /= options.base;
        }
    }

    if (options.allowExp and (s.skip('e') or s.skip('E'))) {
        auto maybeExp = atoi(s, options);
        if (maybeExp)
            exp = maybeExp.unwrap();
    }

    if (not ipart and not hasFpart)
        return NONE;

    auto result = (ipart.unwrapOr(0) + fpart) * Math::pow<f64>(options.base, exp);
    if (neg)
        result = -result;

    rollback.disarm();
    return result;
}

#endif

export template <StaticEncoding E>
Opt<usize> atou(_Str<E> str, AtoxOptions const& options = {}) {
    auto s = _SScan<E>(str);
    return atou(s, options);
}

export template <StaticEncoding E>
Opt<isize> atoi(_Str<E> str, AtoxOptions const& options = {}) {
    auto s = _SScan<E>(str);
    return atoi(s, options);
}

#ifndef __ck_freestanding__

export template <StaticEncoding E>
Opt<f64> atof(_Str<E> str, AtoxOptions const& options = {}) {
    auto s = _SScan<E>(str);
    return atof(s, options);
}

#endif

} // namespace Karm::Io
