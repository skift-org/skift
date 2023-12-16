#pragma once

#include <karm-gfx/buffer.h>
#include <karm-meta/nocopy.h>

namespace Karm::Media {

struct Image {
    Strong<Buf<u8>> _buf;
    Math::Vec2i _size;
    usize _stride;
    Gfx::Fmt _fmt;

    static Image alloc(Math::Vec2i size, Gfx::Fmt fmt = Gfx::RGBA8888) {
        auto buf = makeStrong<Buf<u8>>(Buf<u8>::init(size.x * size.y * fmt.bpp()));
        return {std::move(buf), size, size.x * fmt.bpp(), fmt};
    }

    static Image fallback() {
        auto img = alloc({2, 2}, Gfx::RGBA8888);
        img.mutPixels().clear(Gfx::Color::fromHex(0xFF00FF));
        return img;
    }

    always_inline operator Gfx::Pixels() const {
        return pixels();
    }

    always_inline operator Gfx::MutPixels() {
        return mutPixels();
    }

    always_inline Gfx::Pixels pixels() const {
        return {_buf->buf(), _size, _stride, _fmt};
    }

    always_inline Gfx::MutPixels mutPixels() {
        return {_buf->buf(), _size, _stride, _fmt};
    }

    always_inline isize width() const {
        return _size.x;
    }

    always_inline isize height() const {
        return _size.y;
    }

    always_inline Math::Recti bound() const {
        return {0, 0, width(), height()};
    }

    always_inline Gfx::Color sample(Math::Vec2f pos) const {
        return pixels().sample(pos);
    }
};

} // namespace Karm::Media
