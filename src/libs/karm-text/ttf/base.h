#pragma once

#include <karm-base/endian.h>

namespace Ttf {

struct Fixed {
    u32be value;

    f64 asF64() const {
        return f64(value) / 65536.0;
    }
};

struct Version {
    u16be major;
    u16be minor;
};

} // namespace Ttf
