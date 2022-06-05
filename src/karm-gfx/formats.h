#pragma once

#include "color.h"

namespace Karm::Gfx {

struct Format {
    virtual ~Format() = 0;

    virtual Color load(void const *pixel) = 0;

    virtual void store(void *pixel, Color const color) = 0;

    virtual size_t bpp() = 0;
};

struct RGBA8888 : public Format {
    Color load(void const *pixel) override {
        uint8_t const *p = static_cast<uint8_t const *>(pixel);
        return Color::fromRgba(p[0], p[1], p[2], p[3]);
    }

    void store(void *pixel, Color const color) override {
        uint8_t *p = static_cast<uint8_t *>(pixel);
        p[0] = color.red;
        p[1] = color.green;
        p[2] = color.blue;
        p[3] = color.alpha;
    }

    size_t bpp() override {
        return 4;
    }
};

struct BGRA8888 : public Format {
    Color load(void const *pixel) override {
        uint8_t const *p = static_cast<uint8_t const *>(pixel);
        return Color::fromRgba(p[2], p[1], p[0], p[3]);
    }

    void store(void *pixel, Color const color) override {
        uint8_t *p = static_cast<uint8_t *>(pixel);
        p[0] = color.blue;
        p[1] = color.green;
        p[2] = color.red;
        p[3] = color.alpha;
    }

    size_t bpp() override {
        return 4;
    }
};

} // namespace Karm::Gfx
