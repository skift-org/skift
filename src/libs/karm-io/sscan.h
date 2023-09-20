#pragma once

#include <karm-base/cursor.h>
#include <karm-base/rune.h>
#include <karm-base/string.h>
#include <karm-meta/callable.h>

namespace Karm::Io {

template <typename E>
struct _SScan {
    using Encoding = E;
    using Unit = typename E::Unit;

    Cursor<Unit> _cursor;
    Cursor<Unit> _begin;

    _SScan(Str str) : _cursor(str) {}

    bool ended() {
        return _cursor.ended();
    }

    usize rem() {
        auto curr = _cursor;
        return transcodeLen<E>(curr);
    }

    Rune curr() {
        if (ended()) {
            return '\0';
        }

        Rune r;
        auto curr = _cursor;
        return E::decodeUnit(r, curr) ? r : U'�';
    }

    Rune peek(usize count = 0) {
        auto save = _cursor;
        next(count);
        auto r = curr();
        _cursor = save;
        return r;
    }

    Rune next() {
        if (ended()) {
            return '\0';
        }

        Rune r;
        return E::decodeUnit(r, _cursor) ? r : U'�';
    }

    Rune next(usize count) {
        Rune r = '\0';

        for (usize i = 0; i < count; i++) {
            r = next();
        }
        return r;
    }

    bool skip(Rune c) {
        if (curr() == c) {
            next();
            return true;
        }

        return false;
    }

    bool skip(Str str) {
        auto save = _cursor;

        for (auto r : iterRunes(str)) {
            if (next() != r) {
                _cursor = save;
                return false;
            }
        }

        return true;
    }

    bool skip(auto predicate)
        requires Meta::Callable<decltype(predicate), decltype(*this)>
    {
        auto save = _cursor;
        if (predicate(*this)) {
            return true;
        } else {
            _cursor = save;
            return false;
        }
    }

    bool eat(Rune c) {
        bool result = false;
        while (skip(c)) {
            result = true;
        }
        return result;
    }

    bool eat(auto predicate) {
        bool result = false;
        while (skip(predicate)) {
            result = true;
        }
        return result;
    }

    bool eval(auto expr) {
        return expr(*this);
    }

    bool operator()(auto expr) {
        return expr(*this);
    }

    void begin() {
        _begin = _cursor;
    }

    Str end() {
        return {_begin, _cursor};
    }

    Str token(auto predicate) {
        begin();
        if (not skip(predicate)) {
            _cursor = _begin;
        }
        return end();
    }

    /* --- Number parsing --------------------------------------------------- */

    Opt<u8> _parseDigit(Rune rune, usize base = 10) {
        rune = tolower(rune);
        u8 result = 255;

        if (isalpha(rune))
            result = rune - 'a' + 10;
        else if (isdigit(rune))
            result = rune - '0';

        if (result >= base)
            return NONE;

        return result;
    }

    Opt<u8> nextDigit(usize base = 10) {
        if (ended())
            return NONE;

        auto d = _parseDigit(peek(), base);

        if (d)
            next();

        return d;
    }

    Opt<usize> nextUint(usize base = 10) {
        bool isNum = false;
        usize result = 0;

        while (not ended()) {
            auto maybeDigit = nextDigit(base);
            if (not maybeDigit) {
                break;
            }
            isNum = true;
            result = result * base + maybeDigit.unwrap();
        }

        if (not isNum)
            return NONE;

        return result;
    }

    Opt<isize> nextInt(usize base = 10) {
        bool isNeg = false;
        bool isNum = false;
        isize result = 0;

        if (peek(0) == '-' and _parseDigit(peek(1), base)) {
            isNeg = true;
            isNum = true;
            next();
        }

        while (not ended()) {
            auto maybeDigit = nextDigit(base);
            if (not maybeDigit) {
                break;
            }
            isNum = true;
            result = result * base + maybeDigit.unwrap();
        }

        if (not isNum)
            return NONE;

        if (isNeg)
            result = -result;

        return result;
    }

#ifndef __ck_freestanding__

    Opt<f64> nextFloat(usize base = 10) {
        i64 ipart = 0.0;
        f64 fpart = 0.0;
        i64 exp = 0;

        if (peek(0) != '.' or not _parseDigit(peek(1), base)) {
            ipart = try$(nextInt(base));
        }

        if (skip('.')) {
            f64 multiplier = (1.0 / base);
            while (not ended()) {
                auto maybeDigit = nextDigit(base);
                if (not maybeDigit) {
                    break;
                }
                fpart += maybeDigit.unwrap() * multiplier;
                multiplier /= base;
            }
        }

        if (skip('e') or skip('E')) {
            auto maybeExp = nextInt(base);
            if (maybeExp) {
                exp = maybeExp.unwrap();
            }
        }

        if (ipart < 0) {
            return ipart - fpart * pow(base, exp);
        } else {
            return ipart + fpart * pow(base, exp);
        }
    }

#endif
};

using SScan = _SScan<Utf8>;

} // namespace Karm::Io
