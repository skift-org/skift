#include <karm-base/ring.h>
#include <karm-math/funcs.h>

#include "colors.h"
#include "context.h"

namespace Karm::Gfx {

void Context::begin(Surface c) {
    _surface = c;
    _stack.pushBack({
        .clip = surface().bound(),
    });

    _scanline.resize(c.width());
}

void Context::end() {
    if (_stack.len() != 1) {
        panic("save/restore mismatch");
    }

    _stack.popBack();
    _surface = NONE;
}

Surface &Context::surface() {
    return _surface.unwrap("no surface");
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
    auto copy = current();
    _stack.pushBack(copy);
}

void Context::restore() {
    if (_stack.len() == 1) {
        panic("restore without save");
    }

    _stack.popBack();
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
}

/* --- Transform ------------------------------------------------------------ */

void Context::transform(Math::Trans2f trans) {
    auto &t = current().trans;
    t = t.multiply(trans);
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

void Context::clear(Color color) { clear(surface().bound(), color); }

void Context::clear(Math::Recti rect, Color color) {
    rect = applyAll(rect);
    surface().clear(rect, color);
}

/* --- Blitting ------------------------------------------------------------- */

[[gnu::flatten]] void Context::blit(Math::Recti src, Math::Recti dest, Surface s) {
    dest = applyOrigin(dest);
    auto clipDest = applyAll(dest);

    for (isize y = 0; y < clipDest.height; ++y) {
        isize yy = clipDest.y - dest.y + y;

        auto srcY = src.y + yy * src.height / dest.height;
        auto destY = clipDest.y + y;

        for (isize x = 0; x < clipDest.width; ++x) {
            isize xx = clipDest.x - dest.x + x;

            auto srcX = src.x + xx * src.width / dest.width;
            auto destX = clipDest.x + x;

            auto color = s.load({srcX, srcY});
            surface().blend({destX, destY}, color);
        }
    }
}

void Context::blit(Math::Recti dest, Surface surface) {
    blit(surface.bound(), dest, surface);
}

void Context::blit(Math::Vec2i dest, Surface surface) {
    blit(surface.bound(), {dest, surface.bound().wh}, surface);
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

void Context::fill(Math::Recti r, BorderRadius radius) {
    begin();
    rect(r.cast<f64>(), radius);
    fill();
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
    icon.stroke(*this, pos);
}

void Context::fill(Math::Vec2i pos, Media::Icon icon) {
    icon.fill(*this, pos);
}

void Context::stroke(Math::Vec2i baseline, Rune rune) {
    auto f = textFont();

    save();
    begin();
    origin(baseline.cast<isize>());
    scale(f.fontsize / f.fontface->units());
    f.fontface->contour(*this, rune);
    stroke();
    restore();
}

void Context::fill(Math::Vec2i baseline, Rune rune) {
    auto f = textFont();

    save();
    begin();
    origin(baseline.cast<isize>());
    scale(f.fontsize / f.fontface->units());
    f.fontface->contour(*this, rune);
    fill();
    restore();
}

void Context::stroke(Math::Vec2i baseline, Str str) {
    auto f = textFont();
    for (auto r : iterRunes(str)) {
        stroke(baseline, r);
        baseline.x += f.advance(r);
    }
}

void Context::fill(Math::Vec2i baseline, Str str) {
    auto f = textFont();
    for (auto r : iterRunes(str)) {
        fill(baseline, r);
        baseline.x += f.advance(r);
    }
}

/* --- Debug ---------------------------------------------------------------- */

void Context::debugPlot(Math::Vec2i point, Color color) {
    point = applyOrigin(point);
    if (clip().contains(point)) {
        surface().blend(point, color);
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
    for (auto edge : _shape) {
        debugLine(edge.cast<isize>(), color);
    }
}

/* --- Paths ---------------------------------------------------------------- */

[[gnu::flatten]] void Context::_fillImpl(auto paint, auto format, FillRule fillRule) {

    static constexpr auto AA = 4;
    static constexpr auto UNIT = 1.0f / AA;
    static constexpr auto HALF_UNIT = 1.0f / AA / 2.0;

    auto shapeBound = _shape.bound();
    auto rect = applyClip(shapeBound.ceil().cast<isize>());

    for (isize y = rect.top(); y < rect.bottom(); y++) {
        zeroFill<f64>(mutSub(_scanline, rect.start(), rect.end()));

        for (f64 yy = y; yy < y + 1.0; yy += UNIT) {
            _active.clear();

            for (auto &edge : _shape) {
                auto sample = yy + HALF_UNIT;

                if (edge.bound().top() <= sample and sample < edge.bound().bottom()) {
                    _active.pushBack({
                        .x = edge.sx + (sample - edge.sy) / (edge.ey - edge.sy) * (edge.ex - edge.sx),
                        .sign = edge.sy > edge.ey ? 1 : -1,
                    });
                }
            }

            if (_active.len() == 0)
                continue;

            sort(_active, [](auto const &a, auto const &b) {
                return cmp(a.x, b.x);
            });

            isize rule = 0;
            for (usize i = 0; i + 1 < _active.len(); i++) {
                usize iStart = i;
                usize iEnd = i + 1;

                if (fillRule == FillRule::NONZERO) {
                    isize sign = _active[i].sign;
                    rule += sign;
                    if (rule == 0)
                        continue;
                }

                if (fillRule == FillRule::EVENODD) {
                    rule++;
                    if (rule % 2 == 0)
                        continue;
                }

                f64 x1 = max(_active[iStart].x, rect.start());
                f64 x2 = min(_active[iEnd].x, rect.end());

                if (x1 >= x2) {
                    continue;
                }

                // Are x1 and x2 on the same pixel?
                if (Math::floor(x1) == Math::floor(x2)) {
                    f64 x = Math::floor(x1);
                    _scanline[x] += x2 - x1;
                } else {
                    _scanline[x1] += (ceil(x1) - x1) * UNIT;
                    _scanline[x2] += (x2 - floor(x2)) * UNIT;

                    for (isize x = ceil(x1); x < floor(x2); x++) {
                        _scanline[x] += UNIT;
                    }
                }
            }
        }

        auto *pixel = static_cast<u8 *>(surface().data()) + y * surface().stride() + rect.start() * format.bpp();
        for (isize x = rect.start(); x < rect.end(); x++) {
            Math::Vec2f sample = {
                (x - shapeBound.start()) / shapeBound.width,
                (y - shapeBound.top()) / shapeBound.height,
            };
            auto color = paint.sample(sample);

            auto c = format.load(pixel);
            c = color.withOpacity(clamp01(_scanline[x])).blendOver(c);
            format.store(pixel, c);
            pixel += format.bpp();
        }
    }
}

void Context::_fill(Paint paint, FillRule fillRule) {
    paint.visit([&](auto p) {
        surface().format.visit([&](auto f) {
            _fillImpl(p, f, fillRule);
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
    _shape.clear();
    createSolid(_shape, _path, current().transWithOrigin());
    _fill(paint, rule);
}

void Context::stroke() {
    stroke(strokeStyle());
}

void Context::stroke(StrokeStyle style) {
    _shape.clear();
    createStroke(_shape, _path, style, current().transWithOrigin());
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
    apply(filter, surface().bound());
}

void Context::apply(Filter filter, Math::Recti r) {
    filter.apply(surface().clip(applyAll(r)));
}

} // namespace Karm::Gfx
