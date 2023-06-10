#pragma once

#include <hal/raw.h>

namespace x86_64 {

struct Pit {
    Hal::RawPortIo _io;

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
        return {Hal::RawPortIo({0x40, 4})};
    }

    Res<> init(isize freq) {
        u16 div = FREQ / freq;

        try$(_io.out8(CMD, CHANNEL1 | LOWBYTE | SQUARE_WAVE));
        try$(_io.out8(PORT0, div & 0xFF));
        try$(_io.out8(PORT0, (div >> 8) & 0xFF));

        return Ok();
    }

    Res<u32> readCount() {
        try$(_io.out8(CMD, 0x00));
        u32 low = try$(_io.in8(PORT0));
        u32 high = try$(_io.in8(PORT0));
        return Ok((high << 8) | low);
    }
};

} // namespace x86_64
