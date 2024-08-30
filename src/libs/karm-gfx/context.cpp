#include <karm-base/ring.h>
#include <karm-logger/logger.h>
#include <karm-math/funcs.h>

#include "context.h"
#include "stroke.h"

namespace Karm::Gfx {

// MARK: Buffers ---------------------------------------------------------------

void Context::begin(MutPixels p) {
    _pixels = p;
    _stack.pushBack({
        .clip = pixels().bound(),
    });
}

void Context::end() {
    if (_stack.len() != 1) [[unlikely]]
        panic("save/restore mismatch");

    _stack.popBack();
    _pixels = NONE;
}

MutPixels Context::mutPixels() {
    return _pixels.unwrap("no pixels");
}

// Get the pxeils being drawn on.
Pixels Context::pixels() const {
    return _pixels.unwrap("no pixels");
}

Context::Scope &Context::current() {
    return last(_stack);
}

Context::Scope const &Context::current() const {
    return last(_stack);
}

// MARK: Context Operations ----------------------------------------------------

void Context::push() {
    if (_stack.len() > 100) [[unlikely]]
        panic("context stack overflow");

    _stack.pushBack(current());
}

void Context::pop() {
    if (_stack.len() == 1) [[unlikely]]
        panic("context without save");

    _stack.popBack();
}

void Context::fillStyle(Fill fill) {
    current().fill = fill;
}

void Context::strokeStyle(Stroke style) {
    current().stroke = style;
}

void Context::transform(Math::Trans2f trans) {
    auto &t = current().trans;
    t = trans.multiply(t);
}

// MARK: Path Operations -------------------------------------------------------

void Context::_fillImpl(auto fill, auto format, FillRule fillRule) {
    _rast.fill(_poly, current().clip, fillRule, [&](Rast::Frag frag) {
        auto pixels = mutPixels();
        auto *pixel = pixels.pixelUnsafe(frag.xy);
        auto color = fill.sample(frag.uv);
        auto c = format.load(pixel);
        c = color.withOpacity(frag.a).blendOver(c);
        format.store(pixel, c);
    });
}

void Context::_FillSmoothImpl(auto fill, auto format, FillRule fillRule) {
    Math::Vec2f last = {0, 0};
    auto fillComponent = [&](auto comp, Math::Vec2f pos) {
        _poly.offset(pos - last);
        last = pos;

        _rast.fill(_poly, current().clip, fillRule, [&](Rast::Frag frag) {
            u8 *pixel = static_cast<u8 *>(mutPixels().pixelUnsafe(frag.xy));
            auto color = fill.sample(frag.uv);
            auto c = format.load(pixel);
            c = color.withOpacity(frag.a).blendOverComponent(c, comp);
            format.store(pixel, c);
        });
    };

    fillComponent(Color::RED_COMPONENT, _lcdLayout.red);
    fillComponent(Color::GREEN_COMPONENT, _lcdLayout.green);
    fillComponent(Color::BLUE_COMPONENT, _lcdLayout.blue);
}

void Context::_fill(Fill fill, FillRule fillRule) {
    fill.visit([&](auto fill) {
        pixels().fmt().visit([&](auto format) {
            if (_useSpaa)
                _FillSmoothImpl(fill, format, fillRule);
            else
                _fillImpl(fill, format, fillRule);
        });
    });
}

void Context::beginPath() {
    _path.clear();
}

void Context::closePath() {
    _path.close();
}

void Context::moveTo(Math::Vec2f p, Math::Path::Flags flags) {
    _path.moveTo(p, flags);
}

void Context::lineTo(Math::Vec2f p, Math::Path::Flags flags) {
    _path.lineTo(p, flags);
}

void Context::hlineTo(f64 x, Math::Path::Flags flags) {
    _path.hlineTo(x, flags);
}

void Context::vlineTo(f64 y, Math::Path::Flags flags) {
    _path.vlineTo(y, flags);
}

void Context::cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Math::Path::Flags flags) {
    _path.cubicTo(cp1, cp2, p, flags);
}

void Context::quadTo(Math::Vec2f cp, Math::Vec2f p, Math::Path::Flags flags) {
    _path.quadTo(cp, p, flags);
}

void Context::arcTo(Math::Vec2f radii, f64 angle, Math::Vec2f p, Math::Path::Flags flags) {
    _path.arcTo(radii, angle, p, flags);
}

void Context::line(Math::Edgef line) {
    _path.line(line);
}

void Context::curve(Math::Curvef curve) {
    _path.curve(curve);
}

void Context::rect(Math::Rectf rect, Math::Radiif radii) {
    _path.rect(rect, radii);
}

void Context::path(Math::Path const &path) {
    _path.path(path);
}

void Context::ellipse(Math::Ellipsef ellipse) {
    _path.ellipse(ellipse);
}

void Context::fill(FillRule rule) {
    _poly.clear();
    createSolid(_poly, _path);
    _poly.transform(current().trans);
    _fill(current().fill, rule);
}

void Context::stroke() {
    _poly.clear();
    createStroke(_poly, _path, current().stroke);
    _poly.transform(current().trans);
    _fill(current().stroke.fill);
}

void Context::clip(FillRule) {
    notImplemented();
}

// MARK: Shape Operations ------------------------------------------------------

[[gnu::flatten]] void Context::_fillRect(Math::Recti r, Gfx::Color color) {
    r = current().trans.apply(r.cast<f64>()).cast<isize>();
    r = current().clip.clipTo(r);

    if (color.alpha == 255) {
        mutPixels()
            .clip(r)
            .clear(color);
    } else {
        pixels().fmt().visit([&](auto f) {
            for (isize y = r.y; y < r.y + r.height; ++y) {
                for (isize x = r.x; x < r.x + r.width; ++x) {
                    auto blended = color.blendOver(f.load(mutPixels().pixelUnsafe({x, y})));
                    f.store(mutPixels().pixelUnsafe({x, y}), blended);
                }
            }
        });
    }
}

void Context::fill(Math::Recti r, Math::Radiif radii) {
    beginPath();
    rect(r.cast<f64>(), radii);

    bool isSuitableForFastFill =
        radii.zero() and
        current().fill.is<Color>() and
        current().trans.isIdentity();

    if (isSuitableForFastFill) {
        _fillRect(r, current().fill.unwrap<Color>());
    } else {
        fill(FillRule::NONZERO);
    }
}

void Context::clip(Math::Rectf rect) {
    rect = current().trans.apply(rect.cast<f64>());
    current().clip = rect.cast<isize>().clipTo(current().clip);
}

void Context::stroke(Math::Path const &path) {
    _poly.clear();
    createStroke(_poly, path, current().stroke);
    _poly.transform(current().trans);
    _fill(current().stroke.fill);
}

void Context::fill(Math::Path const &path, FillRule rule) {
    _poly.clear();
    createSolid(_poly, path);
    _poly.transform(current().trans);
    _fill(current().fill, rule);
}

void Context::fill(Text::Font &font, Text::Glyph glyph, Math::Vec2f baseline) {
    _useSpaa = true;
    Canvas::fill(font, glyph, baseline);
    _useSpaa = false;
}

// MARK: Clear Operations ------------------------------------------------------

void Context::clear(Color color) {
    clear(current().clip, color);
}

void Context::clear(Math::Recti rect, Color color) {
    rect = current().trans.apply(rect.cast<f64>()).cast<isize>();
    rect = current().clip.clipTo(rect);
    mutPixels()
        .clip(rect)
        .clear(color);
}

// MARK: Plot Operations ---------------------------------------------------

void Context::plot(Math::Vec2i point, Color color) {
    point = current().trans.apply(point.cast<f64>()).cast<isize>();
    if (current().clip.contains(point)) {
        mutPixels().blend(point, color);
    }
}

void Context::plot(Math::Edgei edge, Color color) {
    isize dx = Math::abs(edge.ex - edge.sx);
    isize sx = edge.sx < edge.ex ? 1 : -1;

    isize dy = -Math::abs(edge.ey - edge.sy);
    isize sy = edge.sy < edge.ey ? 1 : -1;

    isize err = dx + dy, e2;

    for (;;) {
        plot(edge.start, color);
        if (edge.sx == edge.ex and edge.sy == edge.ey)
            break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            edge.sx += sx;
        }
        if (e2 <= dx) {
            err += dx;
            edge.sy += sy;
        }
    }
}

void Context::plot(Math::Recti rect, Color color) {
    rect = {rect.xy, rect.wh - 1};
    plot(Math::Edgei{rect.topStart(), rect.topEnd()}, color);
    plot(Math::Edgei{rect.topEnd(), rect.bottomEnd()}, color);
    plot(Math::Edgei{rect.bottomEnd(), rect.bottomStart()}, color);
    plot(Math::Edgei{rect.bottomStart(), rect.topStart()}, color);
}

// MARK: Blit Operations -------------------------------------------------------

[[gnu::flatten]] void Context::_blit(
    Pixels src, Math::Recti srcRect, auto srcFmt,
    MutPixels dest, Math::Recti destRect, auto destFmt
) {
    destRect = current().trans.apply(destRect.cast<f64>()).cast<isize>();
    auto clipDest = current().clip.clipTo(destRect);

    auto hratio = srcRect.height / (f64)destRect.height;
    auto wratio = srcRect.width / (f64)destRect.width;

    for (isize y = 0; y < clipDest.height; ++y) {
        isize yy = clipDest.y - destRect.y + y;

        auto srcY = srcRect.y + yy * hratio;
        auto destY = clipDest.y + y;

        for (isize x = 0; x < clipDest.width; ++x) {
            isize xx = clipDest.x - destRect.x + x;

            auto srcX = srcRect.x + xx * wratio;
            auto destX = clipDest.x + x;

            u8 const *srcPx = static_cast<u8 const *>(src.pixelUnsafe({(isize)srcX, (isize)srcY}));
            u8 *destPx = static_cast<u8 *>(dest.pixelUnsafe({destX, destY}));
            auto srcC = srcFmt.load(srcPx);
            auto destC = destFmt.load(destPx);
            destFmt.store(destPx, srcC.blendOver(destC));
        }
    }
}

void Context::blit(Math::Recti src, Math::Recti dest, Pixels p) {
    auto d = mutPixels();
    d.fmt().visit([&](auto dfmt) {
        p.fmt().visit([&](auto pfmt) {
            _blit(p, src, pfmt, d, dest, dfmt);
        });
    });
}

// MARK: Filter Operations -----------------------------------------------------

void Context::apply(Filter filter) {
    apply(filter, pixels().bound());
}

void Context::apply(Filter filter, Math::Recti r) {
    r = current().trans.apply(r.cast<f64>()).cast<isize>();
    r = current().clip.clipTo(r);

    filter.apply(mutPixels().clip(r));
}

} // namespace Karm::Gfx
