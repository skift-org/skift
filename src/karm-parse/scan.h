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

    char peek() {
        if (ended()) {
            return '\0';
        }

        return _str[_head];
    }

    char peek(size_t offset) {
        if ((_head + offset) >= _str.len()) {
            return '\0';
        }

        return _str[_head + offset];
    }

    char next() {
        if (ended()) {
            return '\0';
        }

        return _str[_head++];
    }

    char next(size_t offset) {
        if ((_head + offset) >= _str.len()) {
            return '\0';
        }

        return _str[_head + offset];
    }

    bool skip(char c) {
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

        _head += str.len();
        return true;
    }

    bool skip(auto predicate) {
        while (!ended() && predicate(peek())) {
            _head++;
        }

        return !ended();
    }

    bool eat(char c) {
        if (peek() == c) {
            _head++;
            return true;
        }

        return false;
    }

    bool eat(auto predicate) {
        while (!ended() && predicate(peek())) {
            _head++;
        }

        return !ended();
    }

    void begin() {
        _begin = _head;
    }

    Text::Str end() {
        return _str.sub(_begin, _head - _begin);
    }
};

} // namespace Karm::Parse
