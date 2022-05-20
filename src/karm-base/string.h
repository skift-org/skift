#pragma once

#include "clamp.h"
#include "ordr.h"
#include "slice.h"
#include "std.h"

namespace Karm {

using Str = Slice<char>;

using MutStr = MutSlice<char>;

struct String {
    char *_buf = nullptr;
    size_t _len = 0;
    size_t _cap = 0;

    String() = default;

    String(size_t cap) : _cap(cap) {
        _buf = new char[cap];
    }

    String(String const &other) {
        _cap = other._cap;
        _len = other._len;
        _buf = new char[_cap];
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

    operator Str() const {
        return Str(_buf, _len);
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

    char *begin() { return _buf; }

    char const *begin() const { return _buf; }

    char *end() { return _buf + _len; }

    char const *end() const { return _buf + _len; }

    auto iter() { return Slice<char>(_buf, _len).iter(); }

    auto cmp(Str const &other) const {
        return static_cast<Str>(*this).cmp(other);
    }

    char *buf() { return _buf; }

    char const *buf() const { return _buf; }

    size_t len() const { return _len; }

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

    void append(char ch) {
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

    void append(char const *str) {
        append(Str{str});
    }
};

} // namespace Karm
