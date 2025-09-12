module;

#include <karm-gfx/buffer.h>
#include <karm-gfx/colors.h>

export module Karm.Image:qoi.encoder;

import Karm.Core;
import :qoi.base;

namespace Karm::Image::Qoi {

export Res<> encode(Gfx::Pixels pixels, Io::BEmit& e) {
    e.writeBytes(MAGIC);
    e.writeU32be(pixels.width());
    e.writeU32be(pixels.height());
    e.writeU8be(4); // Channels
    e.writeU8be(1); // Color space

    Array<Gfx::Color, 64> index = {};
    Gfx::Color curr = Gfx::BLACK;
    Gfx::Color prev = Gfx::BLACK;

    isize run = 0;

    for (isize y = 0; y < pixels.height(); y++) {
        for (isize x = 0; x < pixels.width(); x++) {
            prev = curr;
            curr = pixels.loadUnsafe({x, y});
            bool end = x == pixels.width() - 1 and
                       y == pixels.height() - 1;

            if (curr == prev) {
                run++;
                if (run == 62 or end) {
                    e.writeU8be(Chunk::RUN | (run - 1));
                    run = 0;
                }
                continue;
            }

            if (run > 0) {
                e.writeU8be(Chunk::RUN | (run - 1));
                run = 0;
            }

            usize index_pos = hashColor(curr) % 64;

            if (index[index_pos] == curr) {
                e.writeU8be(Chunk::INDEX | index_pos);
                continue;
            }

            index[index_pos] = curr;

            if (curr.alpha == prev.alpha) {
                i8 vr = curr.red - prev.red;
                i8 vg = curr.green - prev.green;
                i8 vb = curr.blue - prev.blue;

                i8 vg_r = vr - vg;
                i8 vg_b = vb - vg;

                if (
                    vr > -3 and vr < 2 and
                    vg > -3 and vg < 2 and
                    vb > -3 and vb < 2
                ) {
                    e.writeU8be(Chunk::DIFF | (vr + 2) << 4 | (vg + 2) << 2 | (vb + 2));
                    continue;
                }

                if (
                    vg_r > -9 and vg_r < 8 &&
                    vg > -33 and vg < 32 &&
                    vg_b > -9 and vg_b < 8
                ) {
                    e.writeU8be(Chunk::LUMA | (vg + 32));
                    e.writeU8be((vg_r + 8) << 4 | (vg_b + 8));
                } else {
                    e.writeU8be(Chunk::RGB);
                    e.writeU8be(curr.red);
                    e.writeU8be(curr.green);
                    e.writeU8be(curr.blue);
                }
                continue;
            }

            e.writeU8be(Chunk::RGBA);
            e.writeU8be(curr.red);
            e.writeU8be(curr.green);
            e.writeU8be(curr.blue);
            e.writeU8be(curr.alpha);
        }
    }

    e.writeBytes(END);

    return Ok();
}

} // namespace Karm::Image::Qoi
