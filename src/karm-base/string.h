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

    String() = default;

    String(char const *buf, size_t len) : _len(len) {
        _buf = new char[len + 1];
        _buf[len] = '\0';
        memcpy(_buf, buf, len);
    }

    String(Str str) : String(str.buf(), str.len()) {}

    String(String const &other) : String(other._buf, other._len) {
    }

    String(String &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
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

        return *this;
    }

    char const *begin() const { return _buf; }

    char const *end() const { return _buf + _len; }

    auto iter() const { return Slice<char>(_buf, _len).iter(); }

    auto cmp(Str const &other) const {
        return static_cast<Str>(*this).cmp(other);
    }

    char const *buf() const { return _buf; }

    size_t len() const { return _len; }
};

} // namespace Karm
