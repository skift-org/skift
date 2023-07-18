#include <karm-base/ring.h>
#include <karm-logger/logger.h>
#include <karm-math/funcs.h>

#include "colors.h"
#include "context.h"

namespace Karm::Gfx {

void Context::begin(MutPixels p) {
    _pixels = p;
    _stack.pushBack({
        .clip = pixels().bound(),
    });
    _updateTransform();
}

void Context::end() {
    if (_stack.len() != 1) {
        panic("save/restore mismatch");
    }

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
    if (_stack.len() > 100) {
        panic("save/restore stack overflow");
    }
    _stack.pushBack(current());
}

void Context::restore() {
    if (_stack.len() == 1) {
        panic("restore without save");
    }

    _stack.popBack();
    _updateTransform();
}

/* --- Origin & Clipping ---------------------------------------------------- */

Math::Recti Context::clip() const {
    return current().clip;
}

Math::Vec2i Context::origin() const {
    return current().origin;
}

Math::Recti Context::applyOrigin(Math::Recti rect) const {
    return {rect.xy + origin(), rect.wh};
}

Math::Vec2i Context::applyOrigin(Math::Vec2i pos) const {
    return pos + origin();
}

Math::Recti Context::applyClip(Math::Recti rect) const {
    return rect.clipTo(clip());
}

Math::Recti Context::applyAll(Math::Recti rect) const {
    return applyClip(applyOrigin(rect));
}

void Context::clip(Math::Recti rect) {
    current().clip = applyAll(rect);
}

void Context::origin(Math::Vec2i pos) {
    current().origin = applyOrigin(pos);
    _updateTransform();
}

/* --- Transform ------------------------------------------------------------ */

void Context::transform(Math::Trans2f trans) {
    auto &t = current().trans;
    t = t.multiply(trans);
    _updateTransform();
}

void Context::translate(Math::Vec2f pos) {
    transform(Math::Trans2f::translate(pos.x, pos.y));
}

void Context::scale(Math::Vec2f pos) {
    transform(Math::Trans2f::scale(pos.x, pos.y));
}

void Context::rotate(f64 angle) {
    transform(Math::Trans2f::rotate(angle));
}

void Context::skew(Math::Vec2f pos) {
    transform(Math::Trans2f::skew(pos.x, pos.y));
}

void Context::identity() {
    current().trans = Math::Trans2f::identity();
}

/* --- Fill & Stroke -------------------------------------------------------- */

Paint const &Context::fillStyle() {
    return current().paint;
}

StrokeStyle const &Context::strokeStyle() {
    return current().strokeStyle;
}

Media::Font const &Context::textFont() {
    return current().textFont;
}

ShadowStyle const &Context::shadowStyle() {
    return current().shadowStyle;
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

Context &Context::shadowStyle(ShadowStyle style) {
    current().shadowStyle = style;
    return *this;
}

/* --- Drawing -------------------------------------------------------------- */

void Context::clear(Color color) { clear(pixels().bound(), color); }

void Context::clear(Math::Recti rect, Color color) {
    rect = applyAll(rect);
    mutPixels()
        .clip(rect)
        .clear(color);
}

/* --- Blitting ------------------------------------------------------------- */

[[gnu::flatten]] void Context::_blit(Pixels src, Math::Recti srcRect, auto srcFmt,
                                     MutPixels dest, Math::Recti destRect, auto destFmt) {

    destRect = applyOrigin(destRect);
    auto clipDest = applyClip(destRect);

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

/* --- Shapes --------------------------------------------------------------- */

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

void Context::stroke(Math::Recti r, BorderRadius radius) {
    begin();
    rect(r.cast<f64>(), radius);
    stroke();
}

[[gnu::flatten]] void Context::_fillRect(Math::Recti r, Gfx::Color color) {
    r = applyAll(r);
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

void Context::fill(Math::Recti r, BorderRadius radius) {
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

void Context::stroke(Math::Vec2f baseline, Rune rune) {
    auto f = textFont();

    _useSpaa = true;
    save();
    begin();
    origin(baseline.cast<isize>());
    scale(f.fontsize / f.fontface->units());
    f.fontface->contour(*this, rune);
    stroke();
    restore();
    _useSpaa = false;
}

void Context::fill(Math::Vec2f baseline, Rune rune) {
    auto f = textFont();

    _useSpaa = true;
    save();
    begin();
    origin(baseline.cast<isize>());
    scale(f.fontsize / f.fontface->units());
    f.fontface->contour(*this, rune);
    fill();
    restore();
    _useSpaa = false;
}

void Context::stroke(Math::Vec2f baseline, Str str) {
    auto f = textFont();
    for (auto r : iterRunes(str)) {
        stroke(baseline, r);
        baseline.x += f.advance(r);
    }
}

void Context::fill(Math::Vec2f baseline, Str str) {
    auto f = textFont();

    bool first = true;
    Rune prev = 0;
    for (auto curr : iterRunes(str)) {
        if (not first)
            baseline.x += f.kern(prev, curr);
        fill(baseline, curr);
        baseline.x += f.advance(curr);
        first = false;
        prev = curr;
    }
}

/* --- Debug ---------------------------------------------------------------- */

void Context::debugPlot(Math::Vec2i point, Color color) {
    point = applyOrigin(point);
    if (clip().contains(point)) {
        mutPixels().blend(point, color);
    }
}

void Context::debugLine(Math::Edgei edge, Color color) {
    isize dx = Math::abs(edge.ex - edge.sx);
    isize sx = edge.sx < edge.ex ? 1 : -1;

    isize dy = -Math::abs(edge.ey - edge.sy);
    isize sy = edge.sy < edge.ey ? 1 : -1;

    isize err = dx + dy, e2;

    for (;;) {
        debugPlot({edge.sx, edge.sy}, color);
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

void Context::debugRect(Math::Recti rect, Color color) {
    rect = {rect.xy, rect.wh - 1};
    debugLine({rect.topStart(), rect.topEnd()}, color);
    debugLine({rect.topEnd(), rect.bottomEnd()}, color);
    debugLine({rect.bottomEnd(), rect.bottomStart()}, color);
    debugLine({rect.bottomStart(), rect.topStart()}, color);
}

void Context::debugArrow(Math::Vec2i from, Math::Vec2i to, Color color) {
    const isize SIZE = 16;

    Math::Vec2i dir = to - from;
    Math::Vec2i perp = {-dir.y, dir.x};

    f64 len = dir.len();
    f64 scale = SIZE / len;

    Math::Vec2i p1 = to - dir * scale;
    Math::Vec2i p2 = p1 + perp * scale;
    Math::Vec2i p3 = p1 - perp * scale;

    debugLine({from, to}, color);
    debugLine({to, p2}, color);
    debugLine({to, p3}, color);
}

void Context::debugDoubleArrow(Math::Vec2i from, Math::Vec2i to, Color color) {
    const isize SIZE = 8;

    Math::Vec2f dir = (to - from).cast<f64>();
    Math::Vec2f perp = {-dir.y, dir.x};

    f64 len = dir.len();
    f64 scale = SIZE / len;

    Math::Vec2i p1 = (to - dir * scale).cast<isize>();
    Math::Vec2i p2 = (p1 + perp * scale).cast<isize>();
    Math::Vec2i p3 = (p1 - perp * scale).cast<isize>();

    Math::Vec2i p4 = (from + dir * scale).cast<isize>();
    Math::Vec2i p5 = (p4 + perp * scale).cast<isize>();
    Math::Vec2i p6 = (p4 - perp * scale).cast<isize>();

    debugLine({from, to}, color);
    debugLine({to, p2}, color);
    debugLine({to, p3}, color);
    debugLine({from, p5}, color);
    debugLine({from, p6}, color);
}

void Context::debugTrace(Gfx::Color color) {
    for (auto edge : _rast.shape()) {
        debugLine(edge.cast<isize>(), color);
    }
}

/* --- Paths ---------------------------------------------------------------- */

[[gnu::flatten]] void Context::_fillImpl(auto paint, auto format, FillRule fillRule) {
    _rast.fill(clip(), fillRule, [&](Rast::Frag frag) {
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
        _rast.shape().offset(pos - last);
        last = pos;
        _rast.fill(clip(), fillRule, [&](Rast::Frag frag) {
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

void Context::rect(Math::Rectf rect, BorderRadius radius) {
    _path.rect(rect, radius);
}

void Context::ellipse(Math::Ellipsef ellipse) {
    _path.ellipse(ellipse);
}

void Context::fill(FillRule rule) {
    fill(fillStyle(), rule);
}

void Context::fill(Paint paint, FillRule rule) {
    _rast.clear();
    createSolid(_rast.shape(), _path);
    _fill(paint, rule);
}

void Context::stroke() {
    stroke(strokeStyle());
}

void Context::stroke(StrokeStyle style) {
    _rast.clear();
    createStroke(_rast.shape(), _path, style);
    _fill(style.paint);
}

void Context::shadow() {
    shadow(shadowStyle());
}

void Context::shadow(ShadowStyle style) {
    layer(style.offset, [&](Context &ctx) {
        ctx.fill(style.paint);
        ctx.apply(BlurFilter{(isize)style.radius});
    });
}

/* --- Effects -------------------------------------------------------------- */

void Context::apply(Filter filter) {
    apply(filter, pixels().bound());
}

void Context::apply(Filter filter, Math::Recti r) {
    filter.apply(mutPixels().clip(applyAll(r)));
}

} // namespace Karm::Gfx
