#include <karm-base/ring.h>
#include <karm-logger/logger.h>
#include <karm-math/funcs.h>

#include "context.h"
#include "stroke.h"

namespace Karm::Gfx {

void Context::begin(MutPixels p) {
    _pixels = p;
    _stack.pushBack({
        .clip = pixels().bound(),
    });
    _updateTransform();
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

void Context::save() {
    if (_stack.len() > 100) [[unlikely]]
        panic("save/restore stack overflow");

    _stack.pushBack(current());
}

void Context::restore() {
    if (_stack.len() == 1) [[unlikely]]
        panic("restore without save");

    _stack.popBack();
    _updateTransform();
}

// MARK: Origin & Clipping -----------------------------------------------------

void Context::clip(Math::Recti rect) {
    current().clip = current().trans.apply(rect.cast<f64>()).cast<isize>().clipTo(current().clip);
}

void Context::origin(Math::Vec2i pos) {
    translate(pos.cast<f64>());
}

// MARK: Transform -------------------------------------------------------------

void Context::transform(Math::Trans2f trans) {
    auto &t = current().trans;
    t = trans.multiply(t);
    _updateTransform();
}

void Context::translate(Math::Vec2f pos) {
    transform(Math::Trans2f::translate(pos));
}

void Context::scale(Math::Vec2f pos) {
    transform(Math::Trans2f::scale(pos));
}

void Context::rotate(f64 angle) {
    transform(Math::Trans2f::rotate(angle));
}

void Context::skew(Math::Vec2f pos) {
    transform(Math::Trans2f::skew(pos));
}

void Context::identity() {
    current().trans = Math::Trans2f::identity();
}

// MARK: Fill & Stroke ---------------------------------------------------------

Paint const &Context::fillStyle() {
    return current().paint;
}

StrokeStyle const &Context::strokeStyle() {
    return current().strokeStyle;
}

Media::Font &Context::textFont() {
    return current().textFont;
}

Context &Context::fillStyle(Paint paint) {
    current().paint = paint;
    return *this;
}

Context &Context::strokeStyle(StrokeStyle style) {
    current().strokeStyle = style;
    return *this;
}

Context &Context::textFont(Media::Font font) {
    current().textFont = font;
    return *this;
}

// MARK: Drawing ---------------------------------------------------------------

void Context::clear(Color color) { clear(pixels().bound(), color); }

void Context::clear(Math::Recti rect, Color color) {
    rect = current().trans.apply(rect.cast<f64>()).cast<isize>(),
    mutPixels()
        .clip(rect)
        .clear(color);
}

// MARK: Blitting --------------------------------------------------------------

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

void Context::blit(Math::Recti dest, Pixels pixels) {
    blit(pixels.bound(), dest, pixels);
}

void Context::blit(Math::Vec2i dest, Pixels pixels) {
    blit(pixels.bound(), {dest, pixels.bound().wh}, pixels);
}

// MARK: Shapes ----------------------------------------------------------------

void Context::stroke(Math::Edgei edge) {
    begin();
    moveTo(edge.start.cast<f64>());
    lineTo(edge.end.cast<f64>());
    stroke();
}

void Context::fill(Math::Edgei edge, f64 thickness) {
    begin();
    moveTo(edge.start.cast<f64>());
    lineTo(edge.end.cast<f64>());
    stroke(StrokeStyle().withWidth(thickness));
}

void Context::stroke(Math::Recti r, Math::Radiusf radius) {
    begin();
    rect(r.cast<f64>(), radius);
    stroke();
}

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

void Context::fill(Math::Recti r, Math::Radiusf radius) {
    begin();
    rect(r.cast<f64>(), radius);

    bool isSuitableForFastFill =
        radius.zero() and
        current().paint.is<Color>() and
        current().trans.isIdentity();

    if (isSuitableForFastFill) {
        _fillRect(r, current().paint.unwrap<Color>());
    } else {
        fill();
    }
}

void Context::stroke(Math::Ellipsei e) {
    begin();
    ellipse(e.cast<f64>());
    stroke();
}

void Context::fill(Math::Ellipsei e) {
    begin();
    ellipse(e.cast<f64>());
    fill();
}

void Context::stroke(Math::Vec2i pos, Media::Icon icon) {
    _useSpaa = true;
    icon.stroke(*this, pos);
    _useSpaa = false;
}

void Context::fill(Math::Vec2i pos, Media::Icon icon) {
    _useSpaa = true;
    icon.fill(*this, pos);
    _useSpaa = false;
}

void Context::stroke(Math::Vec2f baseline, Media::Glyph glyph) {
    auto &f = textFont();

    _useSpaa = true;
    save();
    begin();
    translate(baseline);
    scale(f.scale());
    f.fontface->contour(*this, glyph);
    stroke();
    restore();
    _useSpaa = false;
}

void Context::fill(Math::Vec2f baseline, Media::Glyph glyph) {
    auto &f = textFont();

    _useSpaa = true;
    save();
    begin();
    translate(baseline);
    scale(f.scale());
    f.fontface->contour(*this, glyph);
    fill();
    restore();
    _useSpaa = false;
}

void Context::stroke(Math::Vec2f baseline, Rune rune) {
    stroke(baseline, textFont().glyph(rune));
}

void Context::fill(Math::Vec2f baseline, Rune rune) {
    fill(baseline, textFont().glyph(rune));
}

void Context::stroke(Math::Vec2f baseline, Str str) {
    auto &f = textFont();

    bool first = true;
    Media::Glyph prev{0};
    for (auto rune : iterRunes(str)) {
        auto curr = f.glyph(rune);
        if (not first)
            baseline.x += f.kern(prev, curr);
        else
            first = false;

        stroke(baseline, curr);
        baseline.x += f.advance(curr);
        prev = curr;
    }
}

void Context::fill(Math::Vec2f baseline, Str str) {
    auto &f = textFont();

    bool first = true;
    Media::Glyph prev{0};
    for (auto rune : iterRunes(str)) {
        auto curr = f.glyph(rune);
        if (not first)
            baseline.x += f.kern(prev, curr);
        else
            first = false;

        fill(baseline, curr);
        baseline.x += f.advance(curr);
        prev = curr;
    }
}

// MARK: Debug -----------------------------------------------------------------

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

void Context::plot(Gfx::Color color) {
    for (auto edge : _poly) {
        plot(edge.cast<isize>(), color);
    }
}

// MARK: Paths -----------------------------------------------------------------

[[gnu::flatten]] void Context::_fillImpl(auto paint, auto format, FillRule fillRule) {
    _rast.fill(_poly, current().clip, fillRule, [&](Rast::Frag frag) {
        u8 *pixel = static_cast<u8 *>(mutPixels().pixelUnsafe(frag.xy));
        auto color = paint.sample(frag.uv);
        auto c = format.load(pixel);
        c = color.withOpacity(frag.a).blendOver(c);
        format.store(pixel, c);
    });
}

[[gnu::flatten]] void Context::_FillSmoothImpl(auto paint, auto format, FillRule fillRule) {
    Math::Vec2f last = {0, 0};
    auto fillComponent = [&](auto comp, Math::Vec2f pos) {
        _poly.offset(pos - last);
        last = pos;

        _rast.fill(_poly, current().clip, fillRule, [&](Rast::Frag frag) {
            u8 *pixel = static_cast<u8 *>(mutPixels().pixelUnsafe(frag.xy));
            auto color = paint.sample(frag.uv);
            auto c = format.load(pixel);
            c = color.withOpacity(frag.a).blendOverComponent(c, comp);
            format.store(pixel, c);
        });
    };

    fillComponent(Color::RED_COMPONENT, _lcdLayout.red);
    fillComponent(Color::GREEN_COMPONENT, _lcdLayout.green);
    fillComponent(Color::BLUE_COMPONENT, _lcdLayout.blue);
}

void Context::_fill(Paint paint, FillRule fillRule) {
    paint.visit([&](auto paint) {
        pixels().fmt().visit([&](auto format) {
            if (_useSpaa)
                _FillSmoothImpl(paint, format, fillRule);
            else
                _fillImpl(paint, format, fillRule);
        });
    });
}

void Context::begin() {
    _path.clear();
}

void Context::close() {
    _path.close();
}

void Context::moveTo(Math::Vec2f p, Path::Flags flags) {
    _path.moveTo(p, flags);
}

void Context::lineTo(Math::Vec2f p, Path::Flags flags) {
    _path.lineTo(p, flags);
}

void Context::hlineTo(f64 x, Path::Flags flags) {
    _path.hlineTo(x, flags);
}

void Context::vlineTo(f64 y, Path::Flags flags) {
    _path.vlineTo(y, flags);
}

void Context::cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Path::Flags flags) {
    _path.cubicTo(cp1, cp2, p, flags);
}

void Context::quadTo(Math::Vec2f cp, Math::Vec2f p, Path::Flags flags) {
    _path.quadTo(cp, p, flags);
}

void Context::arcTo(Math::Vec2f radius, f64 angle, Math::Vec2f p, Path::Flags flags) {
    _path.arcTo(radius, angle, p, flags);
}

bool Context::evalSvg(Str path) {
    return _path.evalSvg(path);
}

void Context::line(Math::Edgef line) {
    _path.line(line);
}

void Context::rect(Math::Rectf rect, Math::Radiusf radius) {
    _path.rect(rect, radius);
}

void Context::ellipse(Math::Ellipsef ellipse) {
    _path.ellipse(ellipse);
}

void Context::fill(FillRule rule) {
    fill(fillStyle(), rule);
}

void Context::fill(Paint paint, FillRule rule) {
    _poly.clear();
    createSolid(_poly, _path);
    _fill(paint, rule);
}

void Context::stroke() {
    stroke(strokeStyle());
}

void Context::stroke(StrokeStyle style) {
    _poly.clear();
    createStroke(_poly, _path, style);
    _fill(style.paint);
}

// MARK: Effects ---------------------------------------------------------------

void Context::apply(Filter filter) {
    apply(filter, pixels().bound());
}

void Context::apply(Filter filter, Math::Recti r) {
    r = current().trans.apply(r.cast<f64>()).cast<isize>();
    r = current().clip.clipTo(r);

    filter.apply(mutPixels().clip(r));
}

} // namespace Karm::Gfx
