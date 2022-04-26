#pragma once

#include <karm-base/clamp.h>
#include <karm-base/ordr.h>
#include <karm-base/std.h>

#include "rune.h"
#include "utf8.h"

namespace Karm::Text {

struct Str {
    Unit const *_buf;
    size_t _len;

    constexpr Str(Unit const *buf) : _buf(buf), _len(u8strlen(buf)) {}
    constexpr Str(Unit const *buf, size_t len) : _buf(buf), _len(len) {}

    constexpr Unit const *buf() const {
        return _buf;
    }

    constexpr size_t len() const {
        return _len;
    }

    constexpr Str sub(size_t start, size_t end) const {
        return Str(_buf + start, Base::clamp(end, start, _len));
    }

    constexpr Base::Ordr cmp(Str const &other) const {
        return Base::Ordr::from(u8strcmp(_buf, _len, other._buf, other._len));
    }

    constexpr Unit operator[](size_t i) const {
        return _buf[i];
    }

    constexpr Str &operator=(Unit const *rhs) {
        _buf = rhs;
        _len = u8strlen(rhs);
        return *this;
    }
};

} // namespace Karm::Text
