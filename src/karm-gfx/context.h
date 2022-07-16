#pragma once

#include <karm-base/string.h>
#include <karm-base/var.h>
#include <karm-base/vec.h>
#include <karm-math/circle.h>
#include <karm-math/edge.h>
#include <karm-math/funcs.h>

#include "colors.h"
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

struct FillStyle {
    Color _color{};

    FillStyle() : _color(WHITE) {}

    FillStyle(Color color) : _color(color) {}

    Color color() const { return _color; }

    FillStyle color(Color color) {
        _color = color;
        return *this;
    }
};

static inline FillStyle fill(auto... args) {
    return FillStyle(args...);
}

enum struct StrokePosition {
    CENTER,
    INSIDE,
    OUTSIDE,
};

using enum StrokePosition;

struct StrokeStyle {
    Color _color{};
    Radius _radius{};
    StrokePosition _position{};
    double _thickness{};

    StrokeStyle() : _color(WHITE), _thickness(1) {}

    StrokeStyle(Color color) : _color(color), _thickness(1) {}

    Color color() const { return _color; }

    StrokeStyle color(Color color) {
        _color = color;
        return *this;
    }

    Radius radius() const { return _radius; }

    StrokeStyle radius(Radius radius) {
        _radius = radius;
        return *this;
    }

    StrokePosition position() const { return _position; }

    StrokeStyle position(StrokePosition position) {
        _position = position;
        return *this;
    }

    double thickness() const { return _thickness; }

    StrokeStyle thickness(double thickness) {
        _thickness = thickness;
        return *this;
    }
};

static inline StrokeStyle stroke(auto... args) {
    return StrokeStyle(args...);
}

struct TextStyle {
    Color paint{};

    TextStyle() : paint(WHITE) {}

    TextStyle(Color paint) : paint(paint) {}
};

static inline TextStyle text(auto... args) {
    return TextStyle(args...);
}

struct ShadowStyle {
    float spread{};
    float radius{};
    Math::Vec2f offset{};
};

static inline ShadowStyle shadow(auto... args) {
    return ShadowStyle(args...);
}

namespace Sdf {

static inline double circle(Math::Vec2f p, Math::Vec2f center, double radius) {
    return radius - (center - p).len();
}

static inline auto makeCircle(Math::Vec2f center, double radius) {
    return [center, radius](Math::Vec2f p) {
        return circle(p, center, radius);
    };
}

}; // namespace Sdf

struct Context {
    struct Scope {
        FillStyle fillStyle{};
        StrokeStyle strokeStyle{};
        TextStyle textStyle{};
        ShadowStyle shadowStyle{};

        Math::Vec2i origin{};
        Math::Recti clip{};
    };

    Surface *_Surface{};
    Vec<Scope> _stack;

    /* --- Scope --- */

    void begin(Surface &c) {
        _Surface = &c;
        _stack.pushBack({
            .clip = Surface().bound(),
        });
    }

    void end() {
        _stack.popBack();
        _Surface = nullptr;
    }

    Surface &Surface() {
        return *_Surface;
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

    /* --- Orgin & Clipping --- */

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

    /* --- Fill & Stroke --- */

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

    /* --- Drawing --- */

    void clear(Color color = BLACK) { clear(Surface().bound(), color); }

    void clear(Math::Recti rect, Color color = BLACK) {
        rect = applyAll(rect);

        for (int y = rect.y; y < rect.y + rect.height; y++) {
            for (int x = rect.x; x < rect.x + rect.width; x++) {
                Surface().store({x, y}, color);
            }
        }
    }

    void plot(Math::Vec2i point, Color color) {
        point = applyOrigin(point);
        if (clip().contains(point)) {
            Surface().store(point, color);
        }
    }

    void fill(Math::Recti region, Math::Vec2i origin, auto sdf) {
        region = applyAll(region);
        origin = applyOrigin(origin);

        for (int y = region.y; y < region.y + region.height; y++) {
            for (int x = region.x; x < region.x + region.width; x++) {
                auto pos = (Math::Vec2i{x, y} - origin).cast<double>();
                double alpha = clamp(sdf(pos), 0.0, 1.0);

                if (alpha > 0.01) {
                    Color c = fillStyle().color().withOpacity(alpha);
                    Surface().blend({x, y}, c);
                }
            }
        }
    }

    float _clampStroke(float d, float thickness, StrokePosition pos) {
        float inner = 0;
        float outer = -thickness;

        if (pos == StrokePosition::OUTSIDE) {
            inner = thickness;
            outer = 0;
        } else if (pos == StrokePosition::CENTER) {
            inner = thickness / 2;
            outer = -thickness / 2;
        }

        return clamp01(d + inner) - clamp01(d + outer);
    }

    void stroke(Math::Recti region, Math::Vec2i origin, auto sdf) {
        region = applyAll(region);
        origin = applyOrigin(origin);

        for (int y = region.y; y < region.y + region.height; y++) {
            for (int x = region.x; x < region.x + region.width; x++) {
                auto pos = (Math::Vec2i{x, y} - origin).cast<double>();
                float d = sdf(pos);
                double alpha = _clampStroke(d, strokeStyle().thickness(), strokeStyle().position());

                if (alpha > 0.01) {
                    Color c = strokeStyle().color().withOpacity(alpha);
                    Surface().blend({x, y}, c);
                }
            }
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

    void stroke(Math::Circlei circle) {
        double innerRadius = circle.radius;
        double outerRadius = circle.radius;

        switch (strokeStyle().position()) {
        case StrokePosition::CENTER:
            innerRadius -= strokeStyle().thickness() / 2.0f;
            outerRadius += strokeStyle().thickness() / 2.0f;
            break;

        case StrokePosition::INSIDE:
            innerRadius -= strokeStyle().thickness();
            break;

        case StrokePosition::OUTSIDE:
            outerRadius += strokeStyle().thickness();
            break;
        }

        if (innerRadius < 0.0f) {
            innerRadius = 0.0f;
        }

        if (outerRadius < 0.0f) {
            outerRadius = 0.0f;
        }

        if (innerRadius > outerRadius) {
            innerRadius = outerRadius;
        }

        if (innerRadius == outerRadius) {
            return;
        }

        Math::Recti region = {
            (int)Math::floor(circle.center.x - outerRadius),
            (int)Math::floor(circle.center.y - outerRadius),
            (int)Math::ceil(outerRadius * 2.0f),
            (int)Math::ceil(outerRadius * 2.0f),
        };

        stroke(region, circle.center, Sdf::makeCircle({0, 0}, circle.radius));
    }

    void fill(Math::Circlei circle) {
        fill(circle.bound(), circle.center, Sdf::makeCircle({0, 0}, circle.radius));
    }

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

    /* --- Effects --- */

    void blur(Math::Recti region, double radius);

    void saturate(Math::Recti region, double saturation);

    void contrast(Math::Recti region, double contrast);

    void brightness(Math::Recti region, double brightness);

    void noise(Math::Recti region, double amount);

    void sepia(Math::Recti region, double amount);

    void tint(Math::Recti region, Color color);
};

} // namespace Karm::Gfx
