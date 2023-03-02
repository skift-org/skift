#include <karm-base/ring.h>
#include <karm-math/funcs.h>
#include <karm-math/rand.h>

#include "colors.h"
#include "context.h"

namespace Karm::Gfx {

void Context::begin(Surface &c) {
    _surface = &c;
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
    _surface = nullptr;
}

Surface &Context::surface() {
    return *_surface;
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
    _surface->clear(rect, color);
}

/* --- Blitting ------------------------------------------------------------- */

void Context::blit(Math::Recti src, Math::Recti dest, Surface surface) {
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

            auto color = surface.load({srcX, srcY});
            _surface->blend({destX, destY}, color);
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

[[gnu::flatten]] void Context::_fill(Paint paint, FillRule fillRule) {
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

        surface().format.visit([&](auto f) {
            auto *pixel = static_cast<u8 *>(surface().data()) + y * surface().stride() + rect.start() * f.bpp();
            for (isize x = rect.start(); x < rect.end(); x++) {
                Math::Vec2f sample = {
                    (x - shapeBound.start()) / shapeBound.width,
                    (y - shapeBound.top()) / shapeBound.height,
                };
                auto color = paint.sample(sample);

                auto c = f.load(pixel);
                c = color.withOpacity(clamp01(_scanline[x])).blendOver(c);
                f.store(pixel, c);
                pixel += f.bpp();
            }
        });
    }
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

void Context::shadow() {}

/* --- Effects -------------------------------------------------------------- */

struct StackBlur {
    isize _radius;
    Ring<Math::Vec4u> _queue;
    Math::Vec4u _sum;

    StackBlur(isize radius)
        : _radius(radius), _queue(width()) {
        clear();
    }

    Math::Vec4u outgoingSum() const {
        Math::Vec4u sum = {};
        for (isize i = 0; i < _radius; i++) {
            sum = sum + _queue.peek(i);
        }
        return sum;
    }

    Math::Vec4u incomingSum() const {
        Math::Vec4u sum = {};
        for (isize i = 0; i < _radius; i++) {
            sum = sum + _queue.peek(width() - i - 1);
        }
        return sum;
    }

    isize width() const {
        return _radius * 2 + 1;
    }

    isize denominator() const {
        return _radius * (_radius + 2) - 1;
    }

    void enqueue(Math::Vec4u color) {
        _queue.pushBack(color);
        _sum = _sum + incomingSum() - outgoingSum();
    }

    Math::Vec4u dequeue() {
        auto res = _sum / denominator();
        _queue.dequeue();
        return res;
    }

    void clear() {
        _sum = {};
        _queue.clear();
        for (isize i = 0; i < width(); i++) {
            _queue.pushBack({});
        }
    }
};

[[gnu::flatten]] void Context::blur(Math::Recti region, isize radius) {
    if (radius == 0)
        return;

    region = applyClip(region);
    StackBlur stack{radius};

    for (isize y = region.top(); y < region.bottom(); y++) {
        for (isize i = 0; i < stack.width(); i++) {
            auto x = region.start() + i - radius;
            stack.dequeue();
            stack.enqueue(surface().loadClamped({x, y}));
        }

        for (isize x = region.start(); x < region.end(); x++) {
            surface().store({x, y}, stack.dequeue());
            stack.enqueue(surface().loadClamped({x + radius + 1, y}));
        }

        stack.clear();
    }

    for (isize x = region.start(); x < region.end(); x++) {
        for (isize i = 0; i < stack.width(); i++) {
            isize const y = region.top() + i - radius;
            stack.dequeue();
            stack.enqueue(surface().loadClamped({x, y}));
        }

        for (isize y = region.top(); y < region.bottom(); y++) {
            surface().store({x, y}, stack.dequeue());
            stack.enqueue(surface().loadClamped({x, y + radius + 1}));
        }

        stack.clear();
    }
}

void Context::saturate(Math::Recti region, f64 value) {
    region = applyAll(region);

    for (isize y = 0; y < region.height; y++) {
        for (isize x = 0; x < region.width; x++) {
            auto color = _surface->load({region.x + x, region.y + y});

            // weights from CCIR 601 spec
            // https://stackoverflow.com/questions/13806483/increase-or-decrease-color-saturation
            auto gray = 0.2989 * color.red + 0.5870 * color.green + 0.1140 * color.blue;

            u8 red = min(gray * value + color.red * (1 - value), 255);
            u8 green = min(gray * value + color.green * (1 - value), 255);
            u8 blue = min(gray * value + color.blue * (1 - value), 255);

            color = Color::fromRgba(red, green, blue, color.alpha);

            _surface->store({region.x + x, region.y + y}, color);
        }
    }
}

void Context::grayscale(Math::Recti region) {
    region = applyAll(region);

    for (isize y = 0; y < region.height; y++) {
        for (isize x = 0; x < region.width; x++) {
            auto color = _surface->load({region.x + x, region.y + y});

            // weights from CCIR 601 spec
            // https://stackoverflow.com/questions/13806483/increase-or-decrease-color-saturation
            f64 gray = 0.2989 * color.red + 0.5870 * color.green + 0.1140 * color.blue;

            color = Color::fromRgba(gray, gray, gray, color.alpha);
            _surface->store({region.x + x, region.y + y}, color);
        }
    }
}

void Context::contrast(Math::Recti region, f64 contrast) {
    region = applyAll(region);

    contrast *= 255;
    f64 factor = (259 * (contrast + 255)) / (255 * (259 - contrast));

    for (isize y = 0; y < region.height; y++) {
        for (isize x = 0; x < region.width; x++) {
            auto color = _surface->load({region.x + x, region.y + y});

            color = Color::fromRgba(
                min(factor * (color.red - 128) + 128, 255),
                min(factor * (color.green - 128) + 128, 255),
                min(factor * (color.blue - 128) + 128, 255),
                color.alpha);

            _surface->store({region.x + x, region.y + y}, color);
        }
    }
}

void Context::brightness(Math::Recti region, f64 brightness) {
    region = applyAll(region);

    for (isize y = 0; y < region.height; y++) {
        for (isize x = 0; x < region.width; x++) {
            auto color = _surface->load({region.x + x, region.y + y});

            color = Color::fromRgba(
                min(color.red * brightness, 255),
                min(color.green * brightness, 255),
                min(color.blue * brightness, 255),
                color.alpha);

            _surface->store({region.x + x, region.y + y}, color);
        }
    }
}

void Context::noise(Math::Recti region, f64 amount) {
    region = applyAll(region);

    Math::Rand rand{0x12341234};
    u8 alpha = 255 * amount;

    for (isize y = 0; y < region.height; y++) {
        for (isize x = 0; x < region.width; x++) {
            u8 noise = rand.nextU8();

            _surface->blend(
                {region.x + x, region.y + y},
                Color::fromRgba(noise, noise, noise, alpha));
        }
    }
}

void Context::sepia(Math::Recti region, f64 amount) {
    region = applyAll(region);

    for (isize y = 0; y < region.height; y++) {
        for (isize x = 0; x < region.width; x++) {
            auto color = _surface->load({region.x + x, region.y + y});

            auto sepiaColor = Color::fromRgba(
                min((color.red * 0.393) + (color.green * 0.769) + (color.blue * 0.189), 255u),
                min((color.red * 0.349) + (color.green * 0.686) + (color.blue * 0.168), 255u),
                min((color.red * 0.272) + (color.green * 0.534) + (color.blue * 0.131), 255u),
                color.alpha);

            _surface->store({region.x + x, region.y + y}, color.lerpWith(sepiaColor, amount));
        }
    }
}

void Context::tint(Math::Recti region, Color tint) {
    region = applyAll(region);

    for (isize y = 0; y < region.height; y++) {
        for (isize x = 0; x < region.width; x++) {
            auto color = _surface->load({region.x + x, region.y + y});

            auto tintColor = Color::fromRgba(
                (color.red * tint.red) / 255,
                (color.green * tint.green) / 255,
                (color.blue * tint.blue) / 255,
                (color.alpha * tint.alpha) / 255);

            _surface->store({region.x + x, region.y + y}, tintColor);
        }
    }
}

} // namespace Karm::Gfx
