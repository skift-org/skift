#pragma once

#include "clamp.h"
#include "ordr.h"
#include "std.h"

namespace Karm::Base {

using Unit = char8_t;

using Rune = char32_t;

constexpr size_t s8strlen(char const *str) {
    size_t len = 0;
    while (*str) {
        len++;
        str++;
    }
    return len;
}

constexpr size_t u8strlen(char8_t const *str) {
    size_t len = 0;
    while (*str) {
        len++;
        str++;
    }
    return len;
}

constexpr int u8strcmp(char8_t const *lhs, size_t lhs_len, char8_t const *rhs, size_t rhs_len) {
    size_t len = min(lhs_len, rhs_len);
    for (size_t i = 0; i < len; i++) {
        int cmp = lhs[i] - rhs[i];
        if (cmp != 0) {
            return cmp;
        }
    }
    return lhs_len - rhs_len;
}

struct Str {
    Unit const *_buf;
    size_t _len;

    Str(char const *buf) : _buf(reinterpret_cast<Unit const *>(buf)), _len(s8strlen(buf)) {}
    constexpr Str(Unit const *buf) : _buf(buf), _len(u8strlen(buf)) {}
    constexpr Str(Unit const *buf, size_t len) : _buf(buf), _len(len) {}

    constexpr Unit const *buf() const {
        return _buf;
    }

    constexpr size_t len() const {
        return _len;
    }

    constexpr Str sub(size_t start, size_t end) const {
        return Str(_buf + start, clamp(end, start, _len));
    }

    constexpr Ordr cmp(Str const &other) const {
        return Ordr::from(u8strcmp(_buf, _len, other._buf, other._len));
    }

    constexpr Unit operator[](size_t i) const {
        return _buf[i];
    }

    Str &operator=(char const *rhs) {
        _buf = reinterpret_cast<Unit const *>(rhs);
        _len = s8strlen(rhs);
        return *this;
    }

    constexpr Str &operator=(Unit const *rhs) {
        _buf = rhs;
        _len = u8strlen(rhs);
        return *this;
    }
};

struct String {
    char8_t *_buf = nullptr;
    size_t _len = 0;
    size_t _cap = 0;

    String() = default;

    String(size_t cap) : _cap(cap) {
        _buf = new char8_t[cap];
    }

    String(String const &other) {
        _cap = other._cap;
        _len = other._len;
        _buf = new char8_t[_cap];
        for (size_t i = 0; i < _len; i++) {
            _buf[i] = other._buf[i];
        }
    }

    String(String &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
        std::swap(_cap, other._cap);
    }

    ~String() {
        delete[] _buf;
    }

    String &operator=(String const &other) {
        return *this = String(other);
    }

    String &operator=(String &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
        std::swap(_cap, other._cap);

        return *this;
    }

    operator Str() const {
        return Str(_buf, _len);
    }

    char8_t const *buf() const {
        return _buf;
    }

    char8_t *buf() {
        return _buf;
    }

    size_t len() const {
        return _len;
    }

    Ordr cmp(Str const &other) const {
        return Ordr::from(u8strcmp(_buf, _len, other._buf, other._len));
    }

    void ensure(size_t cap) {
        if (cap <= _cap)
            return;

        String tmp(cap);
        for (size_t i = 0; i < _len; i++) {
            tmp._buf[i] = _buf[i];
        }

        std::swap(_buf, tmp._buf);
        std::swap(_len, tmp._len);
        std::swap(_cap, tmp._cap);
    }

    void append(char8_t ch) {
        ensure(_len + 1);

        _buf[_len] = ch;
        _len++;
    }

    void append(Str str) {
        ensure(_len + str.len());
        for (size_t i = 0; i < str.len(); i++) {
            _buf[_len + i] = str[i];
        }
        _len += str.len();
    }

    void append(char8_t const *str) {
        append(Str{str});
    }
};

} // namespace Karm::Base
