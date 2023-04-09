#pragma once

#include <karm-base/rc.h>
#include <karm-base/var.h>
#include <karm-math/rect.h>

#include "color.h"

namespace Karm::Gfx {

struct Rgba8888 {
    ALWAYS_INLINE static Color load(void const *pixel) {
        u8 const *p = static_cast<u8 const *>(pixel);
        return Color::fromRgba(p[0], p[1], p[2], p[3]);
    }

    ALWAYS_INLINE static void store(void *pixel, Color color) {
        u8 *p = static_cast<u8 *>(pixel);
        p[0] = color.red;
        p[1] = color.green;
        p[2] = color.blue;
        p[3] = color.alpha;
    }

    ALWAYS_INLINE static usize bpp() {
        return 4;
    }
};

[[gnu::used]] inline Rgba8888 RGBA8888;

struct Bgra8888 {
    ALWAYS_INLINE static Color load(void const *pixel) {
        u8 const *p = static_cast<u8 const *>(pixel);
        return Color::fromRgba(p[2], p[1], p[0], p[3]);
    }

    ALWAYS_INLINE static void store(void *pixel, Color color) {
        u8 *p = static_cast<u8 *>(pixel);
        p[0] = color.blue;
        p[1] = color.green;
        p[2] = color.red;
        p[3] = color.alpha;
    }

    ALWAYS_INLINE static usize bpp() {
        return 4;
    }
};

[[gnu::used]] inline Bgra8888 BGRA8888;

using _Fmts = Var<Rgba8888, Bgra8888>;

struct Fmt : public _Fmts {
    using _Fmts::_Fmts;

    ALWAYS_INLINE Color load(void const *pixel) const {
        return visit([&](auto f) {
            return f.load(pixel);
        });
    }

    ALWAYS_INLINE void store(void *pixel, Color color) const {
        visit([&](auto f) {
            f.store(pixel, color);
        });
    }

    ALWAYS_INLINE usize bpp() const {
        return visit([&](auto f) {
            return f.bpp();
        });
    }
};

template <bool MUT>
struct _Pixels {
    Meta::Cond<MUT, void *, void const *> _buf;
    Math::Vec2i _size;
    usize _stride;
    Fmt _fmt;

    operator _Pixels<false>() const {
        return {_buf, _size, _stride, _fmt};
    }

    /* --- Geometry --------------------------------------------------------- */

    ALWAYS_INLINE Math::Recti bound() const {
        return {0, 0, _size.x, _size.y};
    }

    ALWAYS_INLINE Math::Vec2i size() const {
        return _size;
    }

    ALWAYS_INLINE isize width() const {
        return _size.x;
    }

    ALWAYS_INLINE isize height() const {
        return _size.y;
    }

    ALWAYS_INLINE usize stride() const {
        return _stride;
    }

    /* --- Buffer Access ---------------------------------------------------- */

    ALWAYS_INLINE Fmt fmt() const {
        return _fmt;
    }

    ALWAYS_INLINE void const *scanline(usize y) const {
        return static_cast<u8 const *>(_buf) + y * _stride;
    }

    ALWAYS_INLINE void *scanline(usize y)
        requires(MUT)
    {
        return static_cast<u8 *>(_buf) + y * _stride;
    }

    ALWAYS_INLINE void const *pixelUnsafe(Math::Vec2i pos) const {
        return static_cast<u8 const *>(_buf) + pos.y * _stride + pos.x * _fmt.bpp();
    }

    ALWAYS_INLINE void *pixelUnsafe(Math::Vec2i pos)
        requires(MUT)
    {
        return static_cast<u8 *>(_buf) + pos.y * _stride + pos.x * _fmt.bpp();
    }

    ALWAYS_INLINE Bytes bytes() const {
        return {static_cast<Byte const *>(_buf), _stride * _size.y};
    }

    ALWAYS_INLINE MutBytes bytes()
        requires(MUT)
    {
        return {static_cast<Byte *>(_buf), _stride * _size.y};
    }

    ALWAYS_INLINE _Pixels<false> clip(Math::Recti rect) const {
        rect = rect.clipTo(bound());

        return {
            pixelUnsafe(rect.xy),
            {rect.width, rect.height},
            _stride,
            _fmt,
        };
    }

    ALWAYS_INLINE _Pixels<MUT> clip(Math::Recti rect)
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

    /* --- Load/Store ------------------------------------------------------- */

    ALWAYS_INLINE Color loadUnsafe(Math::Vec2i pos) const {
        return _fmt.load(pixelUnsafe(pos));
    }

    ALWAYS_INLINE void storeUnsafe(Math::Vec2i pos, Color color)
        requires(MUT)
    {
        _fmt.store(pixelUnsafe(pos), color);
    }

    ALWAYS_INLINE void blendUnsafe(Math::Vec2i pos, Color color)
        requires(MUT)
    {
        storeUnsafe(pos, color.blendOver(loadUnsafe(pos)));
    }

    ALWAYS_INLINE Color load(Math::Vec2i pos) const {
        return loadUnsafe({
            clamp(pos.x, 0, width() - 1),
            clamp(pos.y, 0, height() - 1),
        });
    }

    ALWAYS_INLINE void store(Math::Vec2i pos, Color color)
        requires(MUT)
    {
        storeUnsafe({
                        clamp(pos.x, 0, width() - 1),
                        clamp(pos.y, 0, height() - 1),
                    },
                    color);
    }

    ALWAYS_INLINE void blend(Math::Vec2i pos, Color color)
        requires(MUT)
    {
        blendUnsafe({
                        clamp(pos.x, 0, width() - 1),
                        clamp(pos.y, 0, height() - 1),
                    },
                    color);
    }

    ALWAYS_INLINE Color sample(Math::Vec2f pos) const {
        return load(Math::Vec2i(pos.x * width(), pos.y * height()));
    }

    ALWAYS_INLINE void clear(Color color)
        requires(MUT)
    {
        _fmt.visit([&](auto f) {
            for (isize y = 0; y < height(); y++) {
                for (isize x = 0; x < width(); x++) {
                    f.store(pixelUnsafe({x, y}), color);
                }
            }
        });
    }

    ALWAYS_INLINE void clear()
        requires(MUT)
    {
        clear({});
    }

    ALWAYS_INLINE void blit(Math::Vec2i pos, _Pixels<false> src)
        requires(MUT)
    {
        _fmt.visit([&](auto fd) {
            _fmt.visit([&](auto fs) {
                for (isize y = 0; y < src.height(); y++) {
                    for (isize x = 0; x < src.width(); x++) {
                        fd.store(pixelUnsafe({pos.x + x, pos.y + y}),
                                 fs.load(src.pixelUnsafe({x, y})));
                    }
                }
            });
        });
    }

    ALWAYS_INLINE void blit(Math::Recti dst, _Pixels<false> src) {
        _fmt.visit([&](auto fd) {
            _fmt.visit([&](auto fs) {
                for (isize y = 0; y < dst.height; y++) {
                    for (isize x = 0; x < dst.width; x++) {
                        fd.store(pixelUnsafe({dst.x + x, dst.y + y}),
                                 fs.load(src.pixelUnsafe({x, y})));
                    }
                }
            });
        });
    }
};

using Pixels = _Pixels<false>;

using MutPixels = _Pixels<true>;

} // namespace Karm::Gfx
