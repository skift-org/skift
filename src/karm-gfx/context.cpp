#include <karm-math/funcs.h>

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
    _stack.popBack();
    _surface = nullptr;
}

Surface &Context::surface() {
    return *_surface;
}

Context::Scope &Context::current() {
    return first(_stack);
}

Context::Scope const &Context::current() const {
    return last(_stack);
}

void Context::save() {
    _stack.pushBack(current());
}

void Context::restore() {
    _stack.popBack();
}

/* --- Origin & Clipping ------------------------------------------------ */

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

/* --- Fill & Stroke ---------------------------------------------------- */

FillStyle const &Context::fillStyle() {
    return current().fillStyle;
}

Stroke const &Context::strokeStyle() {
    return current().strokeStyle;
}

Text const &Context::textStyle() {
    return current().textStyle;
}

Media::Font const &Context::textFont() {
    return *current().textStyle.font;
}

Shadow const &Context::shadowStyle() {
    return current().shadowStyle;
}

Context &Context::fillStyle(FillStyle style) {
    current().fillStyle = style;
    return *this;
}

Context &Context::strokeStyle(Stroke style) {
    current().strokeStyle = style;
    return *this;
}

Context &Context::textStyle(Text style) {
    current().textStyle = style;
    return *this;
}

Context &Context::shadowStyle(Shadow style) {
    current().shadowStyle = style;
    return *this;
}

/* --- Drawing ---------------------------------------------------------- */

void Context::clear(Color color) { clear(surface().bound(), color); }

void Context::clear(Math::Recti rect, Color color) {
    rect = applyAll(rect);

    for (int y = rect.y; y < rect.y + rect.height; y++) {
        for (int x = rect.x; x < rect.x + rect.width; x++) {
            surface().store({x, y}, color);
        }
    }
}

/* --- Shapes ----------------------------------------------------------- */

void Context::plot(Math::Vec2i point) {
    plot(point, fillStyle().color());
}

void Context::plot(Math::Vec2i point, Color color) {
    point = applyOrigin(point);
    if (clip().contains(point)) {
        surface().store(point, color);
    }
}

// void Context::stroke(Math::Edgei edge) {}

// void Context::fill(Math::Edgei edge, double thickness = 1.0f) {}

// void Context::stroke(Math::Recti rect) {}

void Context::fill(Math::Recti rect) {
    rect = applyAll(rect);

    for (int y = rect.y; y < rect.y + rect.height; y++) {
        for (int x = rect.x; x < rect.x + rect.width; x++) {
            surface().blend({x, y}, fillStyle().color());
        }
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

/* --- Text ------------------------------------------------------------- */

Media::FontMesure Context::mesureRune(Rune r) {
    return textFont().mesureRune(r);
}

Media::FontMesure Context::mesureStr(Str str) {
    return textFont().mesureStr(str);
}

void Context::strokeRune(Math::Vec2i baseline, Rune rune) {
    textFont().strokeRune(*this, baseline, rune);
}

void Context::fillRune(Math::Vec2i baseline, Rune rune) {
    textFont().fillRune(*this, baseline, rune);
}

void Context::strokeStr(Math::Vec2i baseline, Str str) {
    textFont().strokeStr(*this, baseline, str);
}

void Context::fillStr(Math::Vec2i baseline, Str str) {
    textFont().fillStr(*this, baseline, str);
}

/* --- Paths ------------------------------------------------------------ */

void Context::_line(Math::Edgei edge, Color color) {
    int dx = abs(edge.ex - edge.sx);
    int sx = edge.sx < edge.ex ? 1 : -1;

    int dy = -abs(edge.ey - edge.sy);
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

void Context::_trace() {
    auto b = _shape.bound().cast<int>();
    _line({b.topStart(), b.topEnd()}, MAGENTA);
    _line({b.topEnd(), b.bottomEnd()}, MAGENTA);
    _line({b.bottomEnd(), b.bottomStart()}, MAGENTA);
    _line({b.bottomStart(), b.topStart()}, MAGENTA);

    for (auto edge : _shape) {
        _line(edge.cast<int>(), WHITE);
    }

    for (auto edge : _shape) {
        plot(edge.cast<int>().end, RED);
    }
}

void Context::_fill(Color color) {
    static constexpr auto AA = 4;
    static constexpr auto UNIT = 1.0f / AA;
    static constexpr auto HALF_UNIT = 1.0f / AA / 2.0;

    auto rect = applyClip(_shape.bound().ceil().cast<int>());

    for (int y = rect.top(); y < rect.bottom(); y++) {
        zeroFill<double>(_scanline);

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

            if (_active.len() == 0) {
                continue;
            }

            sort<Active>(_active, [](auto const &a, auto const &b) {
                return cmp(a.x, b.x);
            });

            int r = 0;
            for (size_t i = 0; i + 1 < _active.len(); i++) {
                double x1 = _active[i].x;
                double x2 = _active[i + 1].x;
                int sign = _active[i].sign;

                r += sign;
                if (r == 0) {
                    continue;
                }

                for (double x = max(x1, rect.start()); x < min(x2, rect.end()); x += UNIT) {
                    _scanline[x] += 1.0;
                }
            }

            for (int x = rect.start(); x < rect.end(); x++) {
                if (_scanline[x] > 0) {
                    surface().blend({x, y}, color.withOpacity(_scanline[x] / (AA * AA)));
                }
            }
        }
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

void Context::rect(Math::Rectf rect) {
    _path.rect(rect);
}

void Context::ellipse(Math::Ellipsef ellipse) {
    _path.ellipse(ellipse);
}

void Context::fill() {
    _shape.clear();
    createSolid(_shape, _path);
    _fill(fillStyle().color());
}

void Context::stroke() {
    _shape.clear();
    createStroke(_shape, _path, strokeStyle());
    _fill(strokeStyle().color);
}

void Context::shadow() {}

/* --- Effects ---------------------------------------------------------- */

// void Context::blur(Math::Recti region, double radius) {}

// void Context::saturate(Math::Recti region, double saturation) {}

// void Context::contrast(Math::Recti region, double contrast) {}

// void Context::brightness(Math::Recti region, double brightness) {}

// void Context::noise(Math::Recti region, double amount) {}

// void Context::sepia(Math::Recti region, double amount) {}

// void Context::tint(Math::Recti region, Color color) {}

} // namespace Karm::Gfx
