#pragma once

#include <karm-base/string.h>
#include <karm-base/var.h>
#include <karm-base/vec.h>
#include <karm-math/edge.h>
#include <karm-math/ellipse.h>
#include <karm-math/funcs.h>

#include "colors.h"
#include "path.h"
#include "shape.h"
#include "style.h"
#include "surface.h"
#include "vga-font.h"

namespace Karm::Gfx {

struct Radius {
    double topLeft{};
    double topRight{};
    double bottomLeft{};
    double bottomRight{};

    constexpr Radius() = default;

    constexpr Radius(double all)
        : topLeft(all),
          topRight(all),
          bottomLeft(all),
          bottomRight(all) {}

    constexpr Radius(double topLeft, double topRight, double bottomLeft, double bottomRight)
        : topLeft(topLeft),
          topRight(topRight),
          bottomLeft(bottomLeft),
          bottomRight(bottomRight) {}
};

struct Context {
    struct Scope {
        FillStyle fillStyle{};
        StrokeStyle strokeStyle{};
        TextStyle textStyle{};
        ShadowStyle shadowStyle{};

        Math::Vec2i origin{};
        Math::Recti clip{};
    };

    struct Active {
        float x;
        int sign;
    };

    Surface *_surface{};
    Vec<Scope> _stack{};

    Shape _shape{};
    Path _path{};

    Vec<Active> _active{};
    Vec<double> _scanline;

    /* --- Scope ------------------------------------------------------------ */

    void begin(Surface &c) {
        _surface = &c;
        _stack.pushBack({
            .clip = Surface().bound(),
        });

        _scanline.resize(c.width());
    }

    void end() {
        _stack.popBack();
        _surface = nullptr;
    }

    Surface &Surface() {
        return *_surface;
    }

    Scope &current() {
        return first(_stack);
    }

    Scope const &current() const {
        return last(_stack);
    }

    void save() {
        _stack.pushBack(current());
    }

    void restore() {
        _stack.popBack();
    }

    /* --- Origin & Clipping ------------------------------------------------ */

    Math::Recti clip() const {
        return current().clip;
    }

    Math::Vec2i origin() const {
        return current().origin;
    }

    Math::Recti applyOrigin(Math::Recti rect) const {
        return {rect.xy + origin(), rect.wh};
    }

    Math::Vec2i applyOrigin(Math::Vec2i pos) const {
        return pos + origin();
    }

    Math::Recti applyClip(Math::Recti rect) const {
        return rect.clipTo(clip());
    }

    Math::Recti applyAll(Math::Recti rect) const {
        return applyClip(applyOrigin(rect));
    }

    void clip(Math::Recti rect) {
        current().clip = applyAll(rect);
    }

    void origin(Math::Vec2i pos) {
        current().origin = applyOrigin(pos);
    }

    /* --- Fill & Stroke ---------------------------------------------------- */

    FillStyle const &fillStyle() { return current().fillStyle; }

    StrokeStyle const &strokeStyle() { return current().strokeStyle; }

    TextStyle const &textStyle() { return current().textStyle; }

    ShadowStyle const &shadowStyle() { return current().shadowStyle; }

    Context &fillStyle(FillStyle style) {
        current().fillStyle = style;
        return *this;
    }

    Context &strokeStyle(StrokeStyle style) {
        current().strokeStyle = style;
        return *this;
    }

    Context &textStyle(TextStyle style) {
        current().textStyle = style;
        return *this;
    }

    Context &shadowStyle(ShadowStyle style) {
        current().shadowStyle = style;
        return *this;
    }

    /* --- Drawing ---------------------------------------------------------- */

    void clear(Color color = BLACK) { clear(Surface().bound(), color); }

    void clear(Math::Recti rect, Color color = BLACK) {
        rect = applyAll(rect);

        for (int y = rect.y; y < rect.y + rect.height; y++) {
            for (int x = rect.x; x < rect.x + rect.width; x++) {
                Surface().store({x, y}, color);
            }
        }
    }

    /* --- Shapes ----------------------------------------------------------- */

    void plot(Math::Vec2i point, Color color) {
        point = applyOrigin(point);
        if (clip().contains(point)) {
            Surface().store(point, color);
        }
    }

    void stroke(Math::Edgei edge);

    void fill(Math::Edgei edge, double thickness = 1.0f);

    void stroke(Math::Recti rect);

    void fill(Math::Recti rect) {
        rect = applyAll(rect);

        for (int y = rect.y; y < rect.y + rect.height; y++) {
            for (int x = rect.x; x < rect.x + rect.width; x++) {
                Surface().blend({x, y}, fillStyle().color());
            }
        }
    }

    void stroke(Math::Ellipsei e) {
        begin();
        ellipse(e.cast<double>());
        stroke();
    }

    void fill(Math::Ellipsei e) {
        begin();
        ellipse(e.cast<double>());
        fill();
    }

    /* --- Text ------------------------------------------------------------- */

    Math::Vec2i mesure(Rune) {
        return {VGA_FONT_WIDTH, VGA_FONT_HEIGHT};
    }

    Math::Vec2i mesure(Str text) {
        int width = VGA_FONT_HEIGHT;
        for (auto rune : iterRunes(text)) {
            width += mesure(rune).x;
        }
        return {width, VGA_FONT_HEIGHT};
    }

    void _draw(Math::Vec2i baseline, Rune rune, Color color) {
        One<Ibm437> one;
        encodeOne<Ibm437>(rune, one);

        for (int y = 0; y < VGA_FONT_HEIGHT; y++) {
            for (int x = 0; x < VGA_FONT_WIDTH; x++) {
                uint8_t byte = VGA_FONT[one * VGA_FONT_HEIGHT + y];
                if (byte & (1 << x)) {
                    plot(baseline + Math::Vec2i{x, y - VGA_FONT_HEIGHT}, color);
                }
            }
        }
    }

    void stroke(Math::Vec2i baseline, Rune rune) {
        _draw(baseline, rune, strokeStyle().color());
    }

    void fill(Math::Vec2i baseline, Rune rune) {
        _draw(baseline, rune, fillStyle().color());
    }

    void stroke(Math::Vec2i baseline, Str text) {
        Math::Vec2i pos = baseline;
        for (auto rune : iterRunes(text)) {
            stroke(pos, rune);
            baseline = baseline + Math::Vec2i{VGA_FONT_WIDTH, 0};
        }
    }

    void fill(Math::Vec2i baseline, Str text) {
        Math::Vec2i pos = baseline;
        for (auto rune : iterRunes(text)) {
            fill(pos, rune);
            baseline = baseline + Math::Vec2i{VGA_FONT_WIDTH, 0};
        }
    }

    /* --- Paths ------------------------------------------------------------ */

    void begin() {
        _path.clear();
    }

    void close() {
        _path.close();
    }

    void moveTo(Math::Vec2f p, Path::Flags flags = Path::DEFAULT) {
        _path.moveTo(p, flags);
    }

    void lineTo(Math::Vec2f p, Path::Flags flags = Path::DEFAULT) {
        _path.lineTo(p, flags);
    }

    void hlineTo(double x, Path::Flags flags = Path::DEFAULT) {
        _path.hlineTo(x, flags);
    }

    void vlineTo(double y, Path::Flags flags = Path::DEFAULT) {
        _path.vlineTo(y, flags);
    }

    void cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Path::Flags flags = Path::DEFAULT) {
        _path.cubicTo(cp1, cp2, p, flags);
    }

    void quadTo(Math::Vec2f cp, Math::Vec2f p, Path::Flags flags = Path::DEFAULT) {
        _path.quadTo(cp, p, flags);
    }

    void arcTo(Math::Vec2f radius, float angle, Math::Vec2f p, Path::Flags flags = Path::DEFAULT) {
        _path.arcTo(radius, angle, p, flags);
    }

    void line(Math::Edgef line) {
        _path.line(line);
    }

    void rect(Math::Rectf rect) {
        _path.rect(rect);
    }

    void ellipse(Math::Ellipsef ellipse) {
        _path.ellipse(ellipse);
    }

    void _fill() {
        auto shapeBound = _shape.bound();
        fill(shapeBound.cast<int>());
    }

    void fill() {
        _shape.clear();
        createSolid(_path, _shape);
        _fill();
    }

    void stroke() {
        _shape.clear();
        createStroke(_path, _shape);
        _fill();
    }

    void shadow() {}

    /* --- Effects ---------------------------------------------------------- */

    void blur(Math::Recti region, double radius);

    void saturate(Math::Recti region, double saturation);

    void contrast(Math::Recti region, double contrast);

    void brightness(Math::Recti region, double brightness);

    void noise(Math::Recti region, double amount);

    void sepia(Math::Recti region, double amount);

    void tint(Math::Recti region, Color color);
};

} // namespace Karm::Gfx
