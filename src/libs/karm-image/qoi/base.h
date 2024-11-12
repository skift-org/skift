#pragma once

#include <karm-base/array.h>
#include <karm-gfx/color.h>

namespace Qoi {

// magic "qoif"
static constexpr Array<u8, 4> MAGIC = {
    0x71, 0x6F, 0x69, 0x66
};

static constexpr Array<u8, 8> END = {
    0, 0, 0, 0, 0, 0, 0, 1
};

enum Chunk : u8 {
    RGB = 0b11111110,
    RGBA = 0b11111111,
    INDEX = 0b00000000,
    DIFF = 0b01000000,
    LUMA = 0b10000000,
    RUN = 0b11000000,

    MASK = 0b11000000,
};

static inline usize hashColor(Gfx::Color c) {
    return c.red * 3 + c.green * 5 + c.blue * 7 + c.alpha * 11;
}

} // namespace Qoi
