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

void Context::rotate(double angle) {
    transform(Math::Trans2f::rotate(angle));
}

void Context::skew(Math::Vec2f pos) {
    transform(Math::Trans2f::skew(pos.x, pos.y));
}

void Context::identity() {
    current().trans = Math::Trans2f::identity();
}

/* --- Fill & Stroke -------------------------------------------------------- */

FillStyle const &Context::fillStyle() {
    return current().fillStyle;
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

Context &Context::fillStyle(FillStyle style) {
    current().fillStyle = style;
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
    _surface->clearRect(rect, color);
}

/* --- Blitting ------------------------------------------------------------- */

void Context::blit(Math::Recti src, Math::Recti dest, Surface surface) {
    dest = applyOrigin(dest);
    auto clipDest = applyAll(dest);

    for (int y = 0; y < clipDest.height; ++y) {
        int yy = clipDest.y - dest.y + y;

        auto srcY = src.y + yy * src.height / dest.height;
        auto destY = clipDest.y + y;

        for (int x = 0; x < clipDest.width; ++x) {
            int xx = clipDest.x - dest.x + x;

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

void Context::plot(Math::Vec2i point) {
    plot(point, fillStyle().color);
}

void Context::plot(Math::Vec2i point, Color color) {
    point = applyOrigin(point);
    if (clip().contains(point)) {
        surface().blend(point, color);
    }
}

void Context::stroke(Math::Edgei edge) {
    begin();
    moveTo(edge.start.cast<double>());
    lineTo(edge.end.cast<double>());
    stroke();
}

void Context::fill(Math::Edgei edge, double thickness) {
    begin();
    moveTo(edge.start.cast<double>());
    lineTo(edge.end.cast<double>());
    stroke(StrokeStyle().withWidth(thickness));
}

void Context::stroke(Math::Recti r, BorderRadius radius) {
    begin();
    rect(r.cast<double>(), radius);
    stroke();
}

void Context::fill(Math::Recti r, BorderRadius radius) {
    if (radius.zero() && current().trans.isIdentity()) {
        r = applyAll(r);
        _surface->blendRect(r, fillStyle().color);
    } else {
        begin();
        rect(r.cast<double>(), radius);
        fill();
    }
}

void Context::stroke(Math::Ellipsei e) {
    begin();
    ellipse(e.cast<double>());
    stroke();
}

void Context::fill(Math::Ellipsei e) {
    begin();
    ellipse(e.cast<double>());
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
    origin(baseline.cast<int>());
    scale(f.fontsize / f.fontface->units());
    f.fontface->contour(*this, rune);
    stroke();
    restore();
}

void Context::fill(Math::Vec2i baseline, Rune rune) {
    auto f = textFont();

    save();
    begin();
    origin(baseline.cast<int>());
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

void Context::_line(Math::Edgei edge, Color color) {
    int dx = Math::abs(edge.ex - edge.sx);
    int sx = edge.sx < edge.ex ? 1 : -1;

    int dy = -Math::abs(edge.ey - edge.sy);
    int sy = edge.sy < edge.ey ? 1 : -1;

    int err = dx + dy, e2;

    for (;;) {
        plot({edge.sx, edge.sy}, color);
        if (edge.sx == edge.ex && edge.sy == edge.ey)
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

void Context::_rect(Math::Recti rect, Color color) {
    _line({rect.topStart(), rect.topEnd()}, color);
    _line({rect.topEnd(), rect.bottomEnd()}, color);
    _line({rect.bottomEnd(), rect.bottomStart()}, color);
    _line({rect.bottomStart(), rect.topStart()}, color);
}

void Context::_arrow(Math::Vec2i from, Math::Vec2i to, Color color) {
    const int SIZE = 16;

    Math::Vec2i dir = to - from;
    Math::Vec2i perp = {-dir.y, dir.x};

    double len = dir.len();
    double scale = SIZE / len;

    Math::Vec2i p1 = to - dir * scale;
    Math::Vec2i p2 = p1 + perp * scale;
    Math::Vec2i p3 = p1 - perp * scale;

    _line({from, to}, color);
    _line({to, p2}, color);
    _line({to, p3}, color);
}

void Context::_doubleArrow(Math::Vec2i from, Math::Vec2i to, Color color) {
    const int SIZE = 8;

    Math::Vec2f dir = (to - from).cast<double>();
    Math::Vec2f perp = {-dir.y, dir.x};

    double len = dir.len();
    double scale = SIZE / len;

    Math::Vec2i p1 = (to - dir * scale).cast<int>();
    Math::Vec2i p2 = (p1 + perp * scale).cast<int>();
    Math::Vec2i p3 = (p1 - perp * scale).cast<int>();

    Math::Vec2i p4 = (from + dir * scale).cast<int>();
    Math::Vec2i p5 = (p4 + perp * scale).cast<int>();
    Math::Vec2i p6 = (p4 - perp * scale).cast<int>();

    _line({from, to}, color);

    _line({to, p2}, color);
    _line({to, p3}, color);

    _line({from, p5}, color);
    _line({from, p6}, color);
}

void Context::_trace() {
    auto b = _shape.bound().ceil().cast<int>();
    _rect(b, Gfx::PINK);

    for (auto edge : _shape) {
        _line(edge.cast<int>(), WHITE);
        plot(edge.start.cast<int>(), RED);
    }
}

/* --- Paths ---------------------------------------------------------------- */

[[gnu::flatten]] void Context::_fill(Color color) {
    static constexpr auto AA = 4;
    static constexpr auto UNIT = 1.0f / AA;
    static constexpr auto HALF_UNIT = 1.0f / AA / 2.0;

    auto rect = applyClip(_shape.bound().ceil().cast<int>());

    for (int y = rect.top(); y < rect.bottom(); y++) {
        zeroFill<double>(mutSub(_scanline, rect.start(), rect.end()));

        for (double yy = y; yy < y + 1.0; yy += UNIT) {
            _active.clear();

            for (auto &edge : _shape) {
                auto sample = yy + HALF_UNIT;

                if (edge.bound().top() <= sample && sample < edge.bound().bottom()) {
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

            int rule = 0;
            for (size_t i = 0; i + 1 < _active.len(); i++) {
                size_t iStart = i;
                size_t iEnd = i + 1;

                int sign = _active[i].sign;
                rule += sign;
                if (rule == 0) {
                    continue;
                }

                double x1 = max(_active[iStart].x, rect.start());
                double x2 = min(_active[iEnd].x, rect.end());

                if (x1 >= x2) {
                    continue;
                }

                // Are x1 and x2 on the same pixel?
                if (Math::floor(x1) == Math::floor(x2)) {
                    double x = Math::floor(x1);
                    _scanline[x] += x2 - x1;
                } else {
                    _scanline[x1] += (ceil(x1) - x1) * UNIT;
                    _scanline[x2] += (x2 - floor(x2)) * UNIT;

                    for (int x = ceil(x1); x < floor(x2); x++) {
                        _scanline[x] += UNIT;
                    }
                }
            }
        }

        surface().blendScanline(_scanline.buf(), y, rect.start(), rect.end(), color);
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

void Context::hlineTo(double x, Path::Flags flags) {
    _path.hlineTo(x, flags);
}

void Context::vlineTo(double y, Path::Flags flags) {
    _path.vlineTo(y, flags);
}

void Context::cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Path::Flags flags) {
    _path.cubicTo(cp1, cp2, p, flags);
}

void Context::quadTo(Math::Vec2f cp, Math::Vec2f p, Path::Flags flags) {
    _path.quadTo(cp, p, flags);
}

void Context::arcTo(Math::Vec2f radius, double angle, Math::Vec2f p, Path::Flags flags) {
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

void Context::fill() {
    fill(fillStyle());
}

void Context::fill(FillStyle style) {
    _shape.clear();
    createSolid(_shape, _path, current().transWithOrigin());
    _fill(style.color);
}

void Context::stroke() {
    stroke(strokeStyle());
}

void Context::stroke(StrokeStyle style) {
    _shape.clear();
    createStroke(_shape, _path, style, current().transWithOrigin());
    _fill(style.color);
}

void Context::shadow() {}

/* --- Effects -------------------------------------------------------------- */

struct BlurStack {
    int _radius;
    Ring<Math::Vec4u> _queue;
    Math::Vec4u _sum;

    BlurStack(int radius)
        : _radius(radius), _queue(width()) {
        clear();
    }

    Math::Vec4u outgoingSum() const {
        Math::Vec4u sum = {};
        for (int i = 0; i < _radius; i++) {
            sum = sum + _queue.peek(i);
        }
        return sum;
    }

    Math::Vec4u incomingSum() const {
        Math::Vec4u sum = {};
        for (int i = 0; i < _radius; i++) {
            sum = sum + _queue.peek(width() - i - 1);
        }
        return sum;
    }

    int width() const {
        return _radius * 2 + 1;
    }

    int denominator() const {
        return _radius * (_radius + 2) + 1;
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
        for (int i = 0; i < width(); i++) {
            _queue.pushBack({});
        }
    }
};

[[gnu::flatten]] void Context::blur(Math::Recti region, int radius) {
    if (radius <= 1) {
        return;
    }

    region = applyClip(region);
    BlurStack stack{radius};

    for (int y = region.top(); y < region.bottom(); y++) {
        for (int i = 0; i < stack.width(); i++) {
            auto x = region.start() + i - radius;
            stack.dequeue();
            stack.enqueue(surface().loadClamped({x, y}));
        }

        for (int x = region.start(); x < region.end(); x++) {
            surface().store({x, y}, stack.dequeue());
            stack.enqueue(surface().loadClamped({x + radius + 1, y}));
        }

        stack.clear();
    }

    for (int x = region.start(); x < region.end(); x++) {
        for (int i = 0; i < stack.width(); i++) {
            int const y = region.top() + i - radius;
            stack.dequeue();
            stack.enqueue(surface().loadClamped({x, y}));
        }

        for (int y = region.top(); y < region.bottom(); y++) {
            surface().store({x, y}, stack.dequeue());
            stack.enqueue(surface().loadClamped({x, y + radius + 1}));
        }

        stack.clear();
    }
}

void Context::saturate(Math::Recti region, double value) {
    region = applyAll(region);

    for (int y = 0; y < region.height; y++) {
        for (int x = 0; x < region.width; x++) {
            auto color = _surface->load({region.x + x, region.y + y});

            // weights from CCIR 601 spec
            // https://stackoverflow.com/questions/13806483/increase-or-decrease-color-saturation
            auto gray = 0.2989 * color.red + 0.5870 * color.green + 0.1140 * color.blue;

            uint8_t red = min(gray * value + color.red * (1 - value), 255);
            uint8_t green = min(gray * value + color.green * (1 - value), 255);
            uint8_t blue = min(gray * value + color.blue * (1 - value), 255);

            color = Color::fromRgba(red, green, blue, color.alpha);

            _surface->store({region.x + x, region.y + y}, color);
        }
    }
}

void Context::grayscale(Math::Recti region) {
    region = applyAll(region);

    for (int y = 0; y < region.height; y++) {
        for (int x = 0; x < region.width; x++) {
            auto color = _surface->load({region.x + x, region.y + y});

            // weights from CCIR 601 spec
            // https://stackoverflow.com/questions/13806483/increase-or-decrease-color-saturation
            double gray = 0.2989 * color.red + 0.5870 * color.green + 0.1140 * color.blue;

            color = Color::fromRgba(gray, gray, gray, color.alpha);
            _surface->store({region.x + x, region.y + y}, color);
        }
    }
}

void Context::contrast(Math::Recti region, double contrast) {
    contrast *= 255;
    double factor = (259 * (contrast + 255)) / (255 * (259 - contrast));

    for (int y = 0; y < region.height; y++) {
        for (int x = 0; x < region.width; x++) {
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

void Context::brightness(Math::Recti region, double brightness) {
    for (int y = 0; y < region.height; y++) {
        for (int x = 0; x < region.width; x++) {
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

void Context::noise(Math::Recti region, double amount) {
    Math::Rand rand{0x12341234};
    uint8_t alpha = 255 * amount;

    for (int y = 0; y < region.height; y++) {
        for (int x = 0; x < region.width; x++) {
            uint8_t noise = rand.nextU8();
            plot(
                {region.x + x, region.y + y},
                Color::fromRgba(noise, noise, noise, alpha));
        }
    }
}

void Context::sepia(Math::Recti region, double amount) {
    region = applyAll(region);

    for (int y = 0; y < region.height; y++) {
        for (int x = 0; x < region.width; x++) {
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

    for (int y = 0; y < region.height; y++) {
        for (int x = 0; x < region.width; x++) {
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
