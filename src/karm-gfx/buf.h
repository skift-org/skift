#pragma once

#include <karm-math/rect.h>

#include "color.h"

namespace Karm::Gfx {

struct Buf {
    int width;
    int height;
    int pitch;
    Color *buf;

    Color load(int x, int y) const {
        return buf[y * pitch + x];
    }

    void store(int x, int y, Color color) {
        buf[y * pitch + x] = color;
    }

    void blend(int x, int y, Color color) {
        Color bg = load(x, y);
        store(x, y, color.blend(bg));
    }

    void clear(Color color) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                store(x, y, color);
            }
        }
    }

    Math::Recti bound() const {
        return {0, 0, width, height};
    }
};

} // namespace Karm::Gfx
