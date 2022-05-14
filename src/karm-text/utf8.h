#pragma once

#include <karm-base/string.h>

namespace Karm::Text {

struct Utf8 {
    size_t _len;
    uint8_t _buf[5];

    void reset() {
        _len = 0;
    }

    size_t scalar_len(uint8_t byte) {
        if ((byte & 0xf8) == 0xf0)
            return 4;
        else if ((byte & 0xf0) == 0xe0)
            return 3;
        else if ((byte & 0xe0) == 0xc0)
            return 2;
        else
            return 1;
    }

    Base::Rune decode(uint8_t *buf, size_t len) {
        if (len == 0 || scalar_len(buf[0]) > len) {
            return U'�';
        } else if ((_buf[0] & 0xf8) == 0xf0) {
            return ((0x07 & _buf[0]) << 18) |
                   ((0x3f & _buf[1]) << 12) |
                   ((0x3f & _buf[2]) << 6) |
                   ((0x3f & _buf[3]));
        } else if ((_buf[0] & 0xf0) == 0xe0) {
            return ((0x0f & _buf[0]) << 12) |
                   ((0x3f & _buf[1]) << 6) |
                   ((0x3f & _buf[2]));
        } else if ((_buf[0] & 0xe0) == 0xc0) {
            return ((0x1f & _buf[0]) << 6) |
                   ((0x3f & _buf[1]));
        } else {
            return _buf[0];
        }
    }

    bool decode(uint8_t byte, Base::Rune &rune) {
        _buf[_len++] = byte;

        if (_len == scalar_len(_buf[0])) {
            rune = decode(_buf, _len);
            return true;
        }

        return false;
    }
};

} // namespace Karm::Text
