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

    ALWAYS_INLINE operator Gfx::Pixels() const {
        return pixels();
    }

    ALWAYS_INLINE operator Gfx::MutPixels() {
        return mutPixels();
    }

    ALWAYS_INLINE Gfx::Pixels pixels() const {
        return {_buf->buf(), _size, _stride, _fmt};
    }

    ALWAYS_INLINE Gfx::MutPixels mutPixels() {
        return {_buf->buf(), _size, _stride, _fmt};
    }

    ALWAYS_INLINE isize width() const {
        return _size.x;
    }

    ALWAYS_INLINE isize height() const {
        return _size.y;
    }

    ALWAYS_INLINE Math::Recti bound() const {
        return {0, 0, width(), height()};
    }

    ALWAYS_INLINE Gfx::Color sample(Math::Vec2f pos) const {
        return pixels().sample(pos);
    }
};

} // namespace Karm::Media
