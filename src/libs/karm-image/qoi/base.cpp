module;

#include <karm-gfx/color.h>

export module Karm.Image:qoi.base;

import Karm.Core;

namespace Karm::Image::Qoi {

// magic "qoif"
export constexpr Array<u8, 4> MAGIC = {
    0x71, 0x6F, 0x69, 0x66
};

export constexpr Array<u8, 8> END = {
    0, 0, 0, 0, 0, 0, 0, 1
};

export enum Chunk : u8 {
    RGB = 0b11111110,
    RGBA = 0b11111111,
    INDEX = 0b00000000,
    DIFF = 0b01000000,
    LUMA = 0b10000000,
    RUN = 0b11000000,

    MASK = 0b11000000,
};

export usize hashColor(Gfx::Color c) {
    return c.red * 3 + c.green * 5 + c.blue * 7 + c.alpha * 11;
}

} // namespace Karm::Image::Qoi
