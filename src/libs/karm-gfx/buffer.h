#pragma once

#include <karm-base/rc.h>
#include <karm-base/var.h>
#include <karm-math/rect.h>

#include "color.h"

namespace Karm::Gfx {

struct Rgba8888 {
    static Color load(void const *pixel) {
        u8 const *p = static_cast<u8 const *>(pixel);
        return Color::fromRgba(p[0], p[1], p[2], p[3]);
    }

    static void store(void *pixel, Color const color) {
        u8 *p = static_cast<u8 *>(pixel);
        p[0] = color.red;
        p[1] = color.green;
        p[2] = color.blue;
        p[3] = color.alpha;
    }

    static usize bpp() {
        return 4;
    }
};

[[gnu::used]] inline Rgba8888 RGBA8888;

struct Bgra8888 {
    static Color load(void const *pixel) {
        u8 const *p = static_cast<u8 const *>(pixel);
        return Color::fromRgba(p[2], p[1], p[0], p[3]);
    }

    static void store(void *pixel, Color const color) {
        u8 *p = static_cast<u8 *>(pixel);
        p[0] = color.blue;
        p[1] = color.green;
        p[2] = color.red;
        p[3] = color.alpha;
    }

    static usize bpp() {
        return 4;
    }
};

[[gnu::used]] inline Bgra8888 BGRA8888;

using Format = Var<Rgba8888, Bgra8888>;

struct Buffer {
    void *data;
    isize width;
    isize height;
    usize stride;
};

inline Color load(Format format, void const *pixel) {
    return format.visit([&](auto f) {
        return f.load(pixel);
    });
}

inline void store(Format format, void *pixel, Color const color) {
    format.visit([&](auto f) {
        f.store(pixel, color);
    });
}

inline usize bpp(Format format) {
    return format.visit([&](auto f) {
        return f.bpp();
    });
}

struct Surface {
    Format format;
    Buffer buffer;

    isize width() const { return buffer.width; }

    isize height() const { return buffer.height; }

    usize stride() const { return buffer.stride; }

    void *data() { return buffer.data; }

    void const *data() const { return buffer.data; }

    Math::Recti bound() const {
        return {0, 0, width(), height()};
    }

    void *scanline(usize y) {
        return static_cast<u8 *>(buffer.data) + y * buffer.stride;
    }

    void store(Math::Vec2i pos, Color color) {
        Gfx::store(format, static_cast<u8 *>(buffer.data) + pos.y * buffer.stride + pos.x * bpp(format), color);
    }

    Color load(Math::Vec2i pos) const {
        return Gfx::load(format, static_cast<u8 const *>(buffer.data) + pos.y * buffer.stride + pos.x * bpp(format));
    }

    Color loadClamped(Math::Vec2i pos) const {
        return load({clamp(pos.x, 0, width() - 1), clamp(pos.y, 0, height() - 1)});
    }

    void blend(Math::Vec2i pos, Color color) {
        store(pos, color.blendOver(load(pos)));
    }

    void clear(Math::Recti rect, Color color) {
        format.visit([&](auto f) {
            for (isize y = rect.top(); y < rect.bottom(); y++) {
                for (isize x = rect.start(); x < rect.end(); x++) {
                    f.store(static_cast<u8 *>(buffer.data) + y * buffer.stride + x * f.bpp(), color);
                }
            }
        });
    }
};

} // namespace Karm::Gfx
