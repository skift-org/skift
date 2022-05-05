#pragma once

#include <karm-text/str.h>

namespace Karm::Parse {

struct Scan {
    Text::Str _str;
    size_t _head;
    size_t _begin;

    Scan(Text::Str str) : _str(str), _head(0) {}

    bool ended() {
        return _head >= _str.len();
    }

    Text::Rune peek() {
        if (ended()) {
            return '\0';
        }

        return _str[_head];
    }

    Text::Rune peek(size_t offset) {
        if ((_head + offset) >= _str.len()) {
            return '\0';
        }

        return _str[_head + offset];
    }

    Text::Rune next() {
        if (ended()) {
            return '\0';
        }

        return _str[_head++];
    }

    void next(size_t offset) {
        if ((_head + offset) <= _str.len()) {
            _head += offset;
        }
    }

    bool skip(Text::Rune c) {
        if (peek() == c) {
            _head++;
            return true;
        }

        return false;
    }

    bool skip(Text::Str str) {
        for (size_t i = 0; i < str.len(); i++) {
            if (peek(i) != str[i]) {
                return false;
            }
        }

        next(str.len());
        return true;
    }

    bool skip(auto predicate) {
        if (!ended() && predicate(peek())) {
            _head++;
            return true;
        }

        return false;
    }

    bool eat(Text::Rune c) {
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

    void begin() {
        _begin = _head;
    }

    Text::Str end() {
        return _str.sub(_begin, _head - _begin);
    }
};

} // namespace Karm::Parse
