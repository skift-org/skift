#pragma once

#include "_prelude.h"

#include "string.h"

namespace Karm {

using Rune = uint32_t;

static constexpr size_t rune_len(char byte) {
    if ((byte & 0xf8) == 0xf0)
        return 4;
    else if ((byte & 0xf0) == 0xe0)
        return 3;
    else if ((byte & 0xe0) == 0xc0)
        return 2;
    else
        return 1;
}

static constexpr size_t rune_decode(char const *buf, size_t buf_len) {
    if (buf_len == 0 || rune_len(buf[0]) > buf_len) {
        return U'�';
    } else if ((buf[0] & 0xf8) == 0xf0) {
        return ((0x07 & buf[0]) << 18) |
               ((0x3f & buf[1]) << 12) |
               ((0x3f & buf[2]) << 6) |
               ((0x3f & buf[3]));
    } else if ((buf[0] & 0xf0) == 0xe0) {
        return ((0x0f & buf[0]) << 12) |
               ((0x3f & buf[1]) << 6) |
               ((0x3f & buf[2]));
    } else if ((buf[0] & 0xe0) == 0xc0) {
        return ((0x1f & buf[0]) << 6) |
               ((0x3f & buf[1]));
    } else {
        return buf[0];
    }
}

struct Runes {
    char const *_buf = nullptr;
    size_t _len = 0;

    constexpr Runes &operator++() {
        _buf += rune_len(_buf[0]);
        _len -= rune_len(_buf[0]);
        return *this;
    }

    constexpr Rune operator*() const {
        return rune_decode(_buf, _len);
    }

    constexpr bool operator!=(Runes const &other) const {
        return _buf != other._buf;
    }

    constexpr Runes begin() const {
        return {_buf, _len};
    }

    constexpr Runes end() const {
        return {_buf + _len, 0};
    }
};

constexpr Runes runes(Str const &str) {
    return Runes{str.buf(), str.len()};
}

} // namespace Karm
