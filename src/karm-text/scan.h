#pragma once

#include <karm-base/cursor.h>
#include <karm-base/rune.h>
#include <karm-base/string.h>
#include <karm-meta/callable.h>

namespace Karm::Text {

template <typename E>
struct _Scan {
    using Encoding = E;
    using Unit = typename E::Unit;

    Cursor<Unit> _cursor;
    Cursor<Unit> _begin;

    _Scan(Str str) : _cursor(str) {}

    bool ended() {
        return _cursor.ended();
    }

    size_t rem() {
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

    Rune peek(size_t count = 0) {
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

    Rune next(size_t count) {
        Rune r = '\0';

        for (size_t i = 0; i < count; i++) {
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

    bool skip(auto predicate) requires Meta::Callable<decltype(predicate), Rune> {
        if (!ended() && predicate(curr())) {
            next();
            return true;
        }

        return false;
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

    /* --- Number parsing --------------------------------------------------- */

    Opt<uint8_t> _parseDigit(Rune rune, size_t base = 10) {
        rune = tolower(rune);
        uint8_t result = 255;

        if (isalpha(rune))
            result = rune - 'a' + 10;
        else if (isdigit(rune))
            result = rune - '0';

        if (result >= base)
            return NONE;

        return result;
    }

    Opt<uint8_t> nextDigit(size_t base = 10) {
        if (ended())
            return NONE;

        auto d = _parseDigit(peek(), base);

        if (d)
            next();

        return d;
    }

    Opt<uint64_t> nextUint(size_t base = 10) {
        bool isNum = false;
        uint64_t result = 0;

        while (!ended()) {
            auto maybeDigit = nextDigit(base);
            if (!maybeDigit) {
                break;
            }
            isNum = true;
            result = result * base + maybeDigit.unwrap();
        }

        if (!isNum)
            return NONE;

        return result;
    }

    Opt<int64_t> nextInt(size_t base = 10) {
        bool isNeg = false;
        bool isNum = false;
        int64_t result = 0;

        if (peek(0) == '-' && _parseDigit(peek(1), base)) {
            isNeg = true;
            isNum = true;
            next();
        }

        while (!ended()) {
            auto maybeDigit = nextDigit(base);
            if (!maybeDigit) {
                break;
            }
            isNum = true;
            result = result * base + maybeDigit.unwrap();
        }

        if (!isNum)
            return NONE;

        if (isNeg)
            result = -result;

        return result;
    }

#ifndef __osdk_freestanding__

    Opt<double> nextFloat(size_t base = 10) {
        int64_t ipart = 0.0;
        double fpart = 0.0;
        int64_t exp = 0;

        if (peek(0) != '.' || !_parseDigit(peek(1), base)) {
            ipart = try$(nextInt(base));
        }
        if (skip('.')) {
            double multiplier = (1.0 / base);
            while (!ended()) {
                auto maybeDigit = nextDigit(base);
                if (!maybeDigit) {
                    break;
                }
                fpart += maybeDigit.unwrap() * multiplier;
                multiplier /= base;
            }
        }

        if (skip('e') || skip('E')) {
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

using Scan = _Scan<Utf8>;

} // namespace Karm::Text
