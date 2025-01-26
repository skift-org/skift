#pragma once

#include "sscan.h"

namespace Karm::Io {

struct AtoxOptions {
    u8 base = 10;
    bool allowExp = true;
};

static inline Opt<u8> _parseDigit(Rune rune, AtoxOptions options = {}) {
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
static inline Opt<u8> _nextDigit(_SScan<E>& s, AtoxOptions options = {}) {
    if (s.ended())
        return NONE;

    auto d = _parseDigit(s.peek(), options);

    if (d)
        s.next();

    return d;
}

template <StaticEncoding E>
static inline Opt<usize> atou(_SScan<E>& s, AtoxOptions options = {}) {
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

template <StaticEncoding E>
static inline Opt<isize> atoi(_SScan<E>& s, AtoxOptions options = {}) {
    bool isNeg = false;
    bool isNum = false;
    isize result = 0;

    if (s.peek(0) == '-' and _parseDigit(s.peek(1), options)) {
        isNeg = true;
        isNum = true;
        s.next();
    }

    while (not s.ended()) {
        auto maybeDigit = _nextDigit(s, options);
        if (not maybeDigit) {
            break;
        }
        isNum = true;
        result = result * options.base + maybeDigit.unwrap();
    }

    if (not isNum)
        return NONE;

    if (isNeg)
        result = -result;

    return result;
}

#ifndef __ck_freestanding__

template <StaticEncoding E>
static inline Opt<f64> atof(_SScan<E>& s, AtoxOptions const& options = {}) {
    i64 ipart = 0.0;
    f64 fpart = 0.0;
    i64 exp = 0;

    bool neg = s.skip('-');

    if (s.peek(0) != '.' or not _parseDigit(s.peek(1), options)) {
        ipart = try$(atoi(s, options));
    }

    if (s.skip('.')) {
        f64 multiplier = (1.0 / options.base);
        while (not s.ended()) {
            auto maybeDigit = _nextDigit(s, options);
            if (not maybeDigit)
                break;
            fpart += maybeDigit.unwrap() * multiplier;
            multiplier /= options.base;
        }
    }

    if (options.allowExp and (s.skip('e') or s.skip('E'))) {
        auto maybeExp = atoi(s, options);
        if (maybeExp)
            exp = maybeExp.unwrap();
    }

    auto result = ipart + fpart * pow(options.base, exp);
    if (neg)
        return -result;
    return result;
}

#endif

template <StaticEncoding E>
static inline Opt<usize> atou(_Str<E> str, AtoxOptions const& options = {}) {
    auto s = _SScan<E>(str);
    return atou(s, options);
}

template <StaticEncoding E>
static inline Opt<isize> atoi(_Str<E> str, AtoxOptions const& options = {}) {
    auto s = _SScan<E>(str);
    return atoi(s, options);
}

#ifndef __ck_freestanding__

template <StaticEncoding E>
static inline Opt<f64> atof(_Str<E> str, AtoxOptions const& options = {}) {
    auto s = _SScan<E>(str);
    return atof(s, options);
}

#endif

} // namespace Karm::Io
