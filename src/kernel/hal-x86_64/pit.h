#pragma once

#include <hal/io.h>

namespace x86_64 {

struct Pit {
    Hal::Io _io;

    static Pit pit() {
        return {Hal::Io::port({0x40, 4})};
    }

    void init(int freq) {
        uint16_t div = 1193182 / freq;

        _io.write8(0x3, 0x36);
        _io.write8(0x0, div & 0xFF);
        _io.write8(0x0, (div >> 8) & 0xFF);
    }
};

} // namespace x86_64
