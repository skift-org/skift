#pragma once

#include <karm-math/rect.h>

#include "formats.h"

namespace Karm::Gfx {

struct Surface {
    Format &_format;
    void *_pixels;
    int _width;
    int _height;
    int _stride;

    Surface(Format &format, void *pixels, int width, int height)
        : _format(format),
          _pixels(pixels),
          _width(width),
          _height(height),
          _stride(width * format.bpp()) {}

    Surface(Format &format, void *pixels, int width, int height, int stride)
        : _format(format),
          _pixels(pixels),
          _width(width),
          _height(height),
          _stride(stride) {}

    int width() const {
        return _width;
    }

    int height() const {
        return _height;
    }

    void *data() {
        return _pixels;
    }

    void const *data() const {
        return _pixels;
    }

    Math::Recti bound() const {
        return {0, 0, width(), height()};
    }

    void *scanline(size_t const y) {
        return static_cast<uint8_t *>(_pixels) + y * _stride;
    }

    Color load(Math::Vec2i const pos) {
        return _format.load(static_cast<uint8_t *>(_pixels) + pos.y * _stride + pos.x * _format.bpp());
    }

    void store(Math::Vec2i const pos, Color const color) {
        _format.store(static_cast<uint8_t *>(_pixels) + pos.y * _stride + pos.x * _format.bpp(), color);
    }

    void blend(Math::Vec2i const pos, Color const color) {
        store(pos, color.blendOver(load(pos)));
    }
};

} // namespace Karm::Gfx
