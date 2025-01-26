#include <karm-base/ring.h>
#include <karm-logger/logger.h>
#include <karm-math/funcs.h>

#include "canvas.h"

namespace Karm::Gfx {

// MARK: Buffers ---------------------------------------------------------------

void CpuCanvas::begin(MutPixels p) {
    _pixels = p;
    _stack.pushBack({
        .clip = pixels().bound(),
    });
}

void CpuCanvas::end() {
    if (_stack.len() != 1) [[unlikely]]
        panic("save/restore mismatch");

    _stack.popBack();
    _pixels = NONE;
}

MutPixels CpuCanvas::mutPixels() {
    return _pixels.unwrap("no pixels");
}

// Get the pxeils being drawn on.
Pixels CpuCanvas::pixels() const {
    return _pixels.unwrap("no pixels");
}

CpuCanvas::Scope& CpuCanvas::current() {
    return last(_stack);
}

CpuCanvas::Scope const& CpuCanvas::current() const {
    return last(_stack);
}

// MARK: Context Operations ----------------------------------------------------

void CpuCanvas::push() {
    if (_stack.len() > 100) [[unlikely]]
        panic("context stack overflow");

    _stack.pushBack(current());
}

void CpuCanvas::pop() {
    if (_stack.len() == 1) [[unlikely]]
        panic("context without save");

    _stack.popBack();
}

void CpuCanvas::fillStyle(Fill fill) {
    current().fill = fill;
}

void CpuCanvas::strokeStyle(Stroke style) {
    current().stroke = style;
}

void CpuCanvas::transform(Math::Trans2f trans) {
    auto& t = current().trans;
    t = trans.multiply(t);
}

// MARK: Path Operations -------------------------------------------------------

void CpuCanvas::_fillImpl(auto fill, auto format, FillRule fillRule) {
    _rast.fill(_poly, current().clip, fillRule, [&](CpuRast::Frag frag) {
        auto pixels = mutPixels();
        auto* pixel = pixels.pixelUnsafe(frag.xy);
        auto color = fill.sample(frag.uv);
        auto c = format.load(pixel);
        c = color.withOpacity(frag.a).blendOver(c);
        format.store(pixel, c);
    });
}

void CpuCanvas::_FillSmoothImpl(auto fill, auto format, FillRule fillRule) {
    Math::Vec2f last = {0, 0};
    auto fillComponent = [&](auto comp, Math::Vec2f pos) {
        _poly.offset(pos - last);
        last = pos;

        _rast.fill(_poly, current().clip, fillRule, [&](CpuRast::Frag frag) {
            u8* pixel = static_cast<u8*>(mutPixels().pixelUnsafe(frag.xy));
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

void CpuCanvas::_fill(Fill fill, FillRule fillRule) {
    fill.visit([&](auto fill) {
        pixels().fmt().visit([&](auto format) {
            if (_useSpaa)
                _FillSmoothImpl(fill, format, fillRule);
            else
                _fillImpl(fill, format, fillRule);
        });
    });
}

void CpuCanvas::beginPath() {
    _path.clear();
}

void CpuCanvas::closePath() {
    _path.close();
}

void CpuCanvas::moveTo(Math::Vec2f p, Math::Path::Flags flags) {
    _path.moveTo(p, flags);
}

void CpuCanvas::lineTo(Math::Vec2f p, Math::Path::Flags flags) {
    _path.lineTo(p, flags);
}

void CpuCanvas::hlineTo(f64 x, Math::Path::Flags flags) {
    _path.hlineTo(x, flags);
}

void CpuCanvas::vlineTo(f64 y, Math::Path::Flags flags) {
    _path.vlineTo(y, flags);
}

void CpuCanvas::cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Math::Path::Flags flags) {
    _path.cubicTo(cp1, cp2, p, flags);
}

void CpuCanvas::quadTo(Math::Vec2f cp, Math::Vec2f p, Math::Path::Flags flags) {
    _path.quadTo(cp, p, flags);
}

void CpuCanvas::arcTo(Math::Vec2f radii, f64 angle, Math::Vec2f p, Math::Path::Flags flags) {
    _path.arcTo(radii, angle, p, flags);
}

void CpuCanvas::line(Math::Edgef line) {
    _path.line(line);
}

void CpuCanvas::curve(Math::Curvef curve) {
    _path.curve(curve);
}

void CpuCanvas::rect(Math::Rectf rect, Math::Radiif radii) {
    _path.rect(rect, radii);
}

void CpuCanvas::path(Math::Path const& path) {
    _path.path(path);
}

void CpuCanvas::ellipse(Math::Ellipsef ellipse) {
    _path.ellipse(ellipse);
}

void CpuCanvas::fill(FillRule rule) {
    _poly.clear();
    createSolid(_poly, _path);
    _poly.transform(current().trans);
    _fill(current().fill, rule);
}

void CpuCanvas::stroke() {
    _poly.clear();
    createStroke(_poly, _path, current().stroke);
    _poly.transform(current().trans);
    _fill(current().stroke.fill);
}

void CpuCanvas::clip(FillRule) {
    notImplemented();
}

// MARK: Shape Operations ------------------------------------------------------

[[gnu::flatten]] void CpuCanvas::_fillRect(Math::Recti r, Gfx::Color color) {
    // FIXME: Properly handle offaxis rectangles
    r = current().trans.apply(r.cast<f64>()).bound().cast<isize>();

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

void CpuCanvas::fill(Math::Recti r, Math::Radiif radii) {
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

void CpuCanvas::clip(Math::Rectf rect) {
    // FIXME: Properly handle offaxis rectangles
    rect = current().trans.apply(rect.cast<f64>()).bound();

    current().clip = rect.cast<isize>().clipTo(current().clip);
}

void CpuCanvas::stroke(Math::Path const& path) {
    _poly.clear();
    createStroke(_poly, path, current().stroke);
    _poly.transform(current().trans);
    _fill(current().stroke.fill);
}

void CpuCanvas::fill(Math::Path const& path, FillRule rule) {
    _poly.clear();
    createSolid(_poly, path);
    _poly.transform(current().trans);
    _fill(current().fill, rule);
}

void CpuCanvas::fill(Text::Font& font, Text::Glyph glyph, Math::Vec2f baseline) {
    _useSpaa = true;
    Canvas::fill(font, glyph, baseline);
    _useSpaa = false;
}

// MARK: Clear Operations ------------------------------------------------------

void CpuCanvas::clear(Color color) {
    clear(current().clip, color);
}

void CpuCanvas::clear(Math::Recti rect, Color color) {
    // FIXME: Properly handle offaxis rectangles
    rect = current().trans.apply(rect.cast<f64>()).bound().cast<isize>();

    rect = current().clip.clipTo(rect);
    mutPixels()
        .clip(rect)
        .clear(color);
}

// MARK: Plot Operations ---------------------------------------------------

void CpuCanvas::plot(Math::Vec2i point, Color color) {
    point = current().trans.apply(point.cast<f64>()).cast<isize>();
    if (current().clip.contains(point)) {
        mutPixels().blend(point, color);
    }
}

void CpuCanvas::plot(Math::Edgei edge, Color color) {
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

void CpuCanvas::plot(Math::Recti rect, Color color) {
    rect = {rect.xy, rect.wh - 1};
    plot(Math::Edgei{rect.topStart(), rect.topEnd()}, color);
    plot(Math::Edgei{rect.topEnd(), rect.bottomEnd()}, color);
    plot(Math::Edgei{rect.bottomEnd(), rect.bottomStart()}, color);
    plot(Math::Edgei{rect.bottomStart(), rect.topStart()}, color);
}

// MARK: Blit Operations -------------------------------------------------------

[[gnu::flatten]] void CpuCanvas::_blit(
    Pixels src, Math::Recti srcRect, auto srcFmt,
    MutPixels dest, Math::Recti destRect, auto destFmt
) {
    // FIXME: Properly handle offaxis rectangles
    destRect = current().trans.apply(destRect.cast<f64>()).bound().cast<isize>();

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

            u8 const* srcPx = static_cast<u8 const*>(src.pixelUnsafe({(isize)srcX, (isize)srcY}));
            u8* destPx = static_cast<u8*>(dest.pixelUnsafe({destX, destY}));
            auto srcC = srcFmt.load(srcPx);
            auto destC = destFmt.load(destPx);
            destFmt.store(destPx, srcC.blendOver(destC));
        }
    }
}

void CpuCanvas::blit(Math::Recti src, Math::Recti dest, Pixels p) {
    auto d = mutPixels();
    d.fmt().visit([&](auto dfmt) {
        p.fmt().visit([&](auto pfmt) {
            _blit(p, src, pfmt, d, dest, dfmt);
        });
    });
}

// MARK: Filter Operations -----------------------------------------------------

void CpuCanvas::apply(Filter filter) {
    apply(filter, pixels().bound());
}

void CpuCanvas::apply(Filter filter, Math::Recti r) {
    // FIXME: Properly handle offaxis rectangles
    r = current().trans.apply(r.cast<f64>()).bound().cast<isize>();

    r = current().clip.clipTo(r);

    filter.apply(mutPixels().clip(r));
}

} // namespace Karm::Gfx
