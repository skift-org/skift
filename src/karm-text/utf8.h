#pragma once

#include <karm-base/rune.h>
#include <karm-base/string.h>

namespace Karm::Text {

struct Utf8 {
    size_t _len;
    char _buf[5];

    void reset() {
        _len = 0;
    }

    bool decode(char byte, Base::Rune &rune) {
        _buf[_len++] = byte;

        if (_len == Base::rune_len(_buf[0])) {
            rune = Base::rune_decode(_buf, _len);
            return true;
        }

        return false;
    }
};

} // namespace Karm::Text
