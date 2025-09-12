#pragma once

import Karm.Core;

#include <karm-math/rect.h>

#include "color.h"

namespace Karm::Gfx {

struct Rgba8888 {
    always_inline static Color load(void const* pixel) {
        u8 const* p = static_cast<u8 const*>(pixel);
        return Color::fromRgba(p[0], p[1], p[2], p[3]);
    }

    always_inline static void store(void* pixel, Color color) {
        u8* p = static_cast<u8*>(pixel);
        p[0] = color.red;
        p[1] = color.green;
        p[2] = color.blue;
        p[3] = color.alpha;
    }

    always_inline static constexpr usize bpp() {
        return 4;
    }
};

static inline Rgba8888 RGBA8888;

struct Bgra8888 {
    always_inline static Color load(void const* pixel) {
        u8 const* p = static_cast<u8 const*>(pixel);
        return Color::fromRgba(p[2], p[1], p[0], p[3]);
    }

    always_inline static void store(void* pixel, Color color) {
        u8* p = static_cast<u8*>(pixel);
        p[0] = color.blue;
        p[1] = color.green;
        p[2] = color.red;
        p[3] = color.alpha;
    }

    always_inline static constexpr usize bpp() {
        return 4;
    }
};

static inline Bgra8888 BGRA8888;

struct Greyscale8 {
    always_inline static Color load(void const* pixel) {
        u8 const* p = static_cast<u8 const*>(pixel);
        return Color::fromRgba(p[0], p[0], p[0], 255);
    }

    always_inline static void store(void* pixel, Color color) {
        u8* p = static_cast<u8*>(pixel);
        p[0] = color.red;
    }

    always_inline static constexpr usize bpp() {
        return 1;
    }
};

static inline Greyscale8 GREYSCALE8;

using _Fmts = Union<Rgba8888, Bgra8888, Greyscale8>;

struct Fmt : _Fmts {
    using _Fmts::_Fmts;

    always_inline Color load(void const* pixel) const {
        return visit([&](auto f) {
            return f.load(pixel);
        });
    }

    always_inline void store(void* pixel, Color color) const {
        visit([&](auto f) {
            f.store(pixel, color);
        });
    }

    always_inline constexpr usize bpp() const {
        return visit([&](auto f) {
            return f.bpp();
        });
    }
};

template <bool MUT>
struct _Pixels {
    Meta::Cond<MUT, void*, void const*> _buf;
    Math::Vec2i _size;
    usize _stride;
    Fmt _fmt;

    operator _Pixels<false>() const {
        return {_buf, _size, _stride, _fmt};
    }

    // MARK: Geometry ----------------------------------------------------------

    always_inline Math::Recti bound() const {
        return {0, 0, _size.x, _size.y};
    }

    always_inline Math::Vec2i size() const {
        return _size;
    }

    always_inline isize width() const {
        return _size.x;
    }

    always_inline isize height() const {
        return _size.y;
    }

    always_inline usize stride() const {
        return _stride;
    }

    // MARK: Buffer Access -----------------------------------------------------

    always_inline Fmt fmt() const {
        return _fmt;
    }

    always_inline void const* scanline(usize y) const {
        return static_cast<u8 const*>(_buf) + y * _stride;
    }

    always_inline void* scanline(usize y)
        requires(MUT)
    {
        return static_cast<u8*>(_buf) + y * _stride;
    }

    always_inline void const* pixelUnsafe(Math::Vec2i pos) const {
        return static_cast<u8 const*>(_buf) + pos.y * _stride + pos.x * _fmt.bpp();
    }

    always_inline void* pixelUnsafe(Math::Vec2i pos)
        requires(MUT)
    {
        return static_cast<u8*>(_buf) + pos.y * _stride + pos.x * _fmt.bpp();
    }

    always_inline Bytes bytes() const {
        return {static_cast<u8 const*>(_buf), _stride * _size.y};
    }

    always_inline MutBytes mutBytes()
        requires(MUT)
    {
        return {static_cast<u8*>(_buf), _stride * _size.y};
    }

    always_inline _Pixels<false> clip(Math::Recti rect) const {
        rect = rect.clipTo(bound());

        return {
            pixelUnsafe(rect.xy),
            {rect.width, rect.height},
            _stride,
            _fmt,
        };
    }

    always_inline _Pixels<MUT> clip(Math::Recti rect)
        requires(MUT)
    {
        rect = rect.clipTo(bound());

        return {
            pixelUnsafe(rect.xy),
            {rect.width, rect.height},
            _stride,
            _fmt,
        };
    }

    // MARK: Load/Store --------------------------------------------------------

    always_inline Color loadUnsafe(Math::Vec2i pos) const {
        return _fmt.load(pixelUnsafe(pos));
    }

    always_inline void storeUnsafe(Math::Vec2i pos, Color color)
        requires(MUT)
    {
        _fmt.store(pixelUnsafe(pos), color);
    }

    always_inline void blendUnsafe(Math::Vec2i pos, Color color)
        requires(MUT)
    {
        storeUnsafe(pos, color.blendOver(loadUnsafe(pos)));
    }

    always_inline Color load(Math::Vec2i pos) const {
        return loadUnsafe({
            clamp(pos.x, 0, width() - 1),
            clamp(pos.y, 0, height() - 1),
        });
    }

    always_inline void store(Math::Vec2i pos, Color color)
        requires(MUT)
    {
        storeUnsafe({
                        clamp(pos.x, 0, width() - 1),
                        clamp(pos.y, 0, height() - 1),
                    },
                    color);
    }

    always_inline void blend(Math::Vec2i pos, Color color)
        requires(MUT)
    {
        blendUnsafe({
                        clamp(pos.x, 0, width() - 1),
                        clamp(pos.y, 0, height() - 1),
                    },
                    color);
    }

    always_inline Color sample(Math::Vec2f pos) const {
        return load(Math::Vec2i(pos.x * width(), pos.y * height()));
    }

    always_inline void clear(Color color)
        requires(MUT)
    {
        _fmt.visit([&](auto f) {
            Array<u8, f.bpp()> pixel{};
            f.store(pixel.buf(), color);

            for (isize y = 0; y < height(); y++)
                for (isize x = 0; x < width(); x++)
                    std::memcpy(pixelUnsafe({x, y}), pixel.buf(), f.bpp());
        });
    }

    always_inline void clear()
        requires(MUT)
    {
        clear({});
    }
};

using Pixels = _Pixels<false>;

using MutPixels = _Pixels<true>;

// MARK: Surface --------------------------------------------------------------

struct Surface {
    Buf<u8> _buf;
    Math::Vec2i _size;
    usize _stride;
    Gfx::Fmt _fmt;

    static Rc<Surface> alloc(Math::Vec2i size, Gfx::Fmt fmt = Gfx::RGBA8888) {
        return makeRc<Surface>(
            Buf<u8>::init(size.x * size.y * fmt.bpp()),
            size,
            size.x * fmt.bpp(),
            fmt
        );
    }

    static Rc<Surface> fallback() {
        auto img = alloc({2, 2}, Gfx::RGBA8888);
        img->mutPixels().clear(Gfx::Color::fromHex(0xFF00FF));
        return img;
    }

    always_inline operator Gfx::Pixels() const {
        return pixels();
    }

    always_inline operator Gfx::MutPixels() {
        return mutPixels();
    }

    always_inline Gfx::Pixels pixels() const {
        return {_buf.buf(), _size, _stride, _fmt};
    }

    always_inline Gfx::MutPixels mutPixels() {
        return {_buf.buf(), _size, _stride, _fmt};
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

// MARK: Blitting --------------------------------------------------------------

void blitUnsafe(MutPixels dst, Pixels src);

} // namespace Karm::Gfx
