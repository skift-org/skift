#pragma once

#include <karm-base/string.h>
#include <karm-meta/callable.h>

namespace Karm::Text {

struct Scan {
    Base::Str _str;
    size_t _head;
    size_t _begin;

    Scan(Base::Str str) : _str(str), _head(0) {}

    bool ended() {
        return _head >= _str.len();
    }

    Base::Rune peek() {
        if (ended()) {
            return '\0';
        }

        return _str[_head];
    }

    Base::Rune peek(size_t offset) {
        if ((_head + offset) >= _str.len()) {
            return '\0';
        }

        return _str[_head + offset];
    }

    Base::Rune next() {
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

    bool skip(Base::Rune c) {
        if (peek() == c) {
            _head++;
            return true;
        }

        return false;
    }

    bool skip(Base::Str str) {
        for (size_t i = 0; i < str.len(); i++) {
            if (peek(i) != str[i]) {
                return false;
            }
        }

        next(str.len());
        return true;
    }

    bool skip(auto predicate) requires Meta::Callable<decltype(predicate), Base::Rune> {
        if (!ended() && predicate(peek())) {
            _head++;
            return true;
        }

        return false;
    }

    bool eat(Base::Rune c) {
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

    Base::Str end() {
        return _str.sub(_begin, _head - _begin);
    }
};

} // namespace Karm::Text
