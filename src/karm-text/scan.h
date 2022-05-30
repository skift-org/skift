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
        return transcode_len<E>(curr);
    }

    Rune curr() {
        if (ended()) {
            return '\0';
        }

        Rune r;
        auto curr = _cursor;
        return E::decode(r, curr) ? r : U'�';
    }

    Rune peek(size_t count) {
        if (ended()) {
            return '\0';
        }

        next(count);

        Rune r;
        auto curr = _cursor;
        return E::decode(r, curr) ? r : U'�';
    }

    Rune next() {
        if (ended()) {
            return '\0';
        }

        Rune r;
        return E::decode(r, _cursor) ? r : U'�';
    }

    void next(size_t count) {
        for (size_t i = 0; i < count; i++) {
            next();
        }
    }

    bool skip(Rune c) {
        if (curr() == c) {
            next();
            return true;
        }

        return false;
    }

    bool skip(Str str) {
        size_t i = 0;
        for (auto r : str.runes()) {
            if (peek(i++) != r) {
                return false;
            }
        }

        next(str.len());
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
        return Str(_begin, _cursor);
    }
};

using Scan = _Scan<Utf8>;

} // namespace Karm::Text
