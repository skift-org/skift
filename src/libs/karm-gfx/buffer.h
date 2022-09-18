#pragma once

#include <karm-base/var.h>
#include <karm-math/rect.h>

#include "color.h"

namespace Karm::Gfx {

struct Rgba8888 {
    static Color load(void const *pixel) {
        uint8_t const *p = static_cast<uint8_t const *>(pixel);
        return Color::fromRgba(p[0], p[1], p[2], p[3]);
    }

    static void store(void *pixel, Color const color) {
        uint8_t *p = static_cast<uint8_t *>(pixel);
        p[0] = color.red;
        p[1] = color.green;
        p[2] = color.blue;
        p[3] = color.alpha;
    }

    static size_t bpp() {
        return 4;
    }
};

[[gnu::used]] static inline Rgba8888 RGBA8888;

struct Bgra8888 {
    static Color load(void const *pixel) {
        uint8_t const *p = static_cast<uint8_t const *>(pixel);
        return Color::fromRgba(p[2], p[1], p[0], p[3]);
    }

    static void store(void *pixel, Color const color) {
        uint8_t *p = static_cast<uint8_t *>(pixel);
        p[0] = color.blue;
        p[1] = color.green;
        p[2] = color.red;
        p[3] = color.alpha;
    }

    static size_t bpp() {
        return 4;
    }
};

[[gnu::used]] static inline Bgra8888 BGRA8888;

using Format = Var<Rgba8888, Bgra8888>;

struct Buffer {
    void *pixels;
    int width;
    int height;
    int stride;
};

static inline Color load(Format format, void const *pixel) {
    return format.visit([&](auto f) {
        return f.load(pixel);
    });
}

static inline void store(Format format, void *pixel, Color const color) {
    format.visit([&](auto f) {
        f.store(pixel, color);
    });
}

static inline size_t bpp(Format format) {
    return format.visit([&](auto f) {
        return f.bpp();
    });
}

struct Surface {
    Format format;
    Buffer buffer;

    int width() const {
        return buffer.width;
    }

    int height() const {
        return buffer.height;
    }

    void *data() {
        return buffer.pixels;
    }

    void const *data() const {
        return buffer.pixels;
    }

    Math::Recti bound() const {
        return {0, 0, width(), height()};
    }

    void *scanline(size_t const y) {
        return static_cast<uint8_t *>(buffer.pixels) + y * buffer.stride;
    }

    void store(Math::Vec2i const pos, Color const color) {
        Gfx::store(format, static_cast<uint8_t *>(buffer.pixels) + pos.y * buffer.stride + pos.x * bpp(format), color);
    }

    Color load(Math::Vec2i const pos) const {
        return Gfx::load(format, static_cast<uint8_t const *>(buffer.pixels) + pos.y * buffer.stride + pos.x * bpp(format));
    }

    void blend(Math::Vec2i const pos, Color const color) {
        store(pos, color.blendOver(load(pos)));
    }

    void clearRect(Math::Recti rect, Color color) {
        format.visit([&](auto f) {
            for (int y = rect.top(); y < rect.bottom(); y++) {
                for (int x = rect.start(); x < rect.end(); x++) {
                    f.store(static_cast<uint8_t *>(buffer.pixels) + y * buffer.stride + x * f.bpp(), color);
                }
            }
        });
    }

    void blendRect(Math::Recti rect, Color color) {
        if (color.alpha == 0) {
            return;
        }

        if (color.alpha == 255) {
            clearRect(rect, color);
            return;
        }

        format.visit([&](auto f) {
            for (int y = rect.top(); y < rect.bottom(); y++) {
                for (int x = rect.start(); x < rect.end(); x++) {
                    auto *pixel = static_cast<uint8_t *>(buffer.pixels) + y * buffer.stride + x * f.bpp();
                    auto c = f.load(pixel);
                    c = color.blendOver(c);
                    f.store(pixel, c);
                }
            }
        });
    }

    void blendScanline(double *alphas, int y, int start, int end, int aa, Color color) {
        format.visit([&](auto f) {
            auto *pixel = static_cast<uint8_t *>(buffer.pixels) + y * buffer.stride + start * f.bpp();
            for (int x = start; x < end; x++) {
                auto c = f.load(pixel);
                auto alpha = (alphas[x]) / (aa * aa);
                c = color.withOpacity(alpha * alpha).blendOver(c);
                f.store(pixel, c);
                pixel += f.bpp();
            }
        });
    }
};

} // namespace Karm::Gfx
