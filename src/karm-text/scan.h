#pragma once

#include <karm-base/rune.h>
#include <karm-base/string.h>
#include <karm-meta/callable.h>

namespace Karm::Text {

struct Scan {
    Str _str;
    size_t _head;
    size_t _begin;

    Scan(Str str) : _str(str), _head(0) {}

    bool ended() {
        return _head >= _str.len();
    }

    size_t rem() {
        return _str.len() - _head;
    }

    Rune peek() {
        if (ended()) {
            return '\0';
        }

        return rune_decode(_str.buf() + _head, rem());
    }

    Rune peek(size_t count) {
        char const *buf = _str.buf() + _head;

        while (count-- > 0 && buf < _str.end()) {
            buf += rune_len(*buf);
        }

        if (buf >= _str.end()) {
            return '\0';
        }

        return rune_decode(buf, rem());
    }

    Rune next() {
        if (ended()) {
            return '\0';
        }

        Rune c = rune_decode(_str.buf() + _head, rem());
        _head += rune_len(c);
        return c;
    }

    void next(size_t count) {
        for (size_t i = 0; i < count; i++) {
            next();
        }
    }

    bool skip(Rune c) {
        if (peek() == c) {
            _head++;
            return true;
        }

        return false;
    }

    bool skip(Str str) {
        size_t i = 0;
        for (auto r : runes(str)) {
            if (peek(i++) != r) {
                return false;
            }
        }

        next(str.len());
        return true;
    }

    bool skip(auto predicate) requires Meta::Callable<decltype(predicate), Rune> {
        if (!ended() && predicate(peek())) {
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
        _begin = _head;
    }

    Str end() {
        return _str.sub(_begin, _head - _begin);
    }
};

} // namespace Karm::Text
