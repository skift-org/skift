#pragma once

#include <karm-gfx/buffer.h>
#include <karm-meta/nocopy.h>

namespace Karm::Media {

struct ImageBuffer :
    public Meta::NoCopy {
    uint8_t *data;
    int width;
    int height;
    size_t stride;

    ImageBuffer(int width, int height, size_t stride)
        : data(new uint8_t[height * stride]), width(width), height(height), stride(stride) {
    }

    ~ImageBuffer() {
        delete[] data;
    }

    operator Gfx::Buffer() {
        return {data, width, height, stride};
    }
};

struct Image {
    Gfx::Format format;
    Strong<ImageBuffer> buffer;

    Image(Gfx::Format format, Math::Vec2i size)
        : format(format), buffer(makeStrong<ImageBuffer>(size.x, size.y, size.x * bpp(format))) {
    }

    operator Gfx::Surface() {
        return {format, *buffer};
    }

    int width() const {
        return buffer->width;
    }

    int height() const {
        return buffer->height;
    }

    Math::Recti bound() const {
        return {0, 0, width(), height()};
    }
};

} // namespace Karm::Media
