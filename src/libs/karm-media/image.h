#pragma once

#include <karm-gfx/buffer.h>
#include <karm-meta/nocopy.h>

namespace Karm::Media {

struct ImageBuffer :
    public Meta::NoCopy {
    Buf<u8> data;
    isize width;
    isize height;
    usize stride;

    ImageBuffer(isize width, isize height, usize stride)
        : data(Buf<u8>::init(height * stride)), width(width), height(height), stride(stride) {
    }

    operator Gfx::Buffer() {
        return {data.buf(), width, height, stride};
    }
};

struct Image {
    Gfx::Format format;
    Strong<ImageBuffer> buffer;

    Image(Gfx::Format format, Math::Vec2i size)
        : format(format), buffer(makeStrong<ImageBuffer>(size.x, size.y, size.x * bpp(format))) {
    }

    operator Gfx::Surface() const {
        return {format, *buffer};
    }

    Gfx::Surface surface() const {
        return *this;
    }

    isize width() const {
        return buffer->width;
    }

    isize height() const {
        return buffer->height;
    }

    Math::Recti bound() const {
        return {0, 0, width(), height()};
    }

    ALWAYS_INLINE Gfx::Color sample(Math::Vec2f pos) const {
        Gfx::Surface s = *this;
        return s.loadClamped(Math::Vec2i(pos.x * width(), pos.y * height()));
    }
};

} // namespace Karm::Media
