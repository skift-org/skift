#pragma once

#include <hal/io.h>

namespace x86_64 {

struct Pit {
    Hal::Io _io;

    // Constants
    static constexpr auto USEC = 1000;
    static constexpr auto FREQ = 1193182;

    // Registers
    static constexpr auto PORT0 = 0;
    static constexpr auto PORT1 = 1;
    static constexpr auto PORT2 = 2;
    static constexpr auto CMD = 3;

    static constexpr auto CHANNEL1 = 1 << 5;
    static constexpr auto LOWBYTE = 1 << 4;
    static constexpr auto SQUARE_WAVE = 6;

    static Pit pit() {
        return {Hal::Io::port({0x40, 4})};
    }

    void init(isize freq) {
        u16 div = FREQ / freq;

        _io.write8(CMD, CHANNEL1 | LOWBYTE | SQUARE_WAVE);
        _io.write8(PORT0, div & 0xFF);
        _io.write8(PORT0, (div >> 8) & 0xFF);
    }

    u32 readCount() {
        _io.write8(CMD, 0x00);
        u32 low = _io.read8(PORT0);
        u32 high = _io.read8(PORT0);
        return (high << 8) | low;
    }
};

} // namespace x86_64
