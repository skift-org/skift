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

struct FillStyle {
    Color color;
};

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

struct StrokeStyle {
    enum struct Position {
        CENTER,
        INSIDE,
        OUTSIDE,
    };

    using enum Position;

    Color color;
    Radius radius;
    Position position;
    double thickness;
};

struct TextStyle {
    Color paint;
};

namespace Sdf {

static inline double circle(Math::Vec2f p, Math::Vec2f center, double radius) {
    return (p - center).len() - radius;
}

static inline auto circle(Math::Vec2f p, Math::Vec2f center, double innerRadius, double outerRadius) {
    return circle(p, center, outerRadius) - circle(p, center, innerRadius);
}

static inline auto makeCircle(Math::Vec2f center, double radius) {
    return [center, radius](Math::Vec2f p) {
        return circle(p, center, radius);
    };
}

static inline auto makeCircle(Math::Vec2f center, double innerRadius, double outerRadius) {
    return [center, innerRadius, outerRadius](Math::Vec2f p) {
        return circle(p, center, innerRadius, outerRadius);
    };
};

}; // namespace Sdf

struct Context {
    struct Scope {
        FillStyle fillStyle{};
        StrokeStyle strokeStyle{};
        TextStyle textStyle{};

        Math::Vec2i origin{};
        Math::Recti clip{};
    };

    Surface *_Surface{};
    Vec<Scope> _stack;

    /* --- Scope --- */

    void begin(Surface &c) {
        _Surface = &c;
        _stack.pushBack({
            .fillStyle = {
                .color = Colors::WHITE,
            },
            .strokeStyle = {
                .color = Colors::WHITE,
                .radius = {0, 0, 0, 0},
                .position = StrokeStyle::Position::CENTER,
                .thickness = 1,
            },
            .textStyle = {
                .paint = Colors::WHITE,
            },
            .origin = {0, 0},
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
        return _stack.peekBack();
    }

    Scope const &current() const {
        return _stack.peekBack();
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

    void fillStyle(FillStyle style) { current().fillStyle = style; }

    void strokeStyle(StrokeStyle style) { current().strokeStyle = style; }

    void textStyle(TextStyle style) { current().textStyle = style; }

    /* --- Drawing --- */

    void clear() { clear(Colors::BLACK); }

    void clear(Color color) { clear(Surface().bound(), color); }

    void clear(Math::Recti rect, Color color) {
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
                    Color c = fillStyle().color.withOpacity(alpha);
                    Surface().blend({x, y}, c);
                }
            }
        }
    }

    void stroke(Math::Recti region, Math::Vec2i origin, auto sdf) {
        region = applyAll(region);
        origin = applyOrigin(origin);

        for (int y = region.y; y < region.y + region.height; y++) {
            for (int x = region.x; x < region.x + region.width; x++) {
                auto pos = (Math::Vec2i{x, y} - origin).cast<double>();
                double alpha = clamp(sdf(pos), 0.0, 1.0);

                if (alpha > 0.01) {
                    Color c = strokeStyle().color.withOpacity(alpha);
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
                Surface().blend({x, y}, fillStyle().color);
            }
        }
    }

    void stroke(Math::Circlei circle) {
        double innerRadius = circle.radius;
        double outerRadius = circle.radius;

        switch (strokeStyle().position) {
        case StrokeStyle::Position::CENTER:
            innerRadius -= strokeStyle().thickness / 2.0f;
            outerRadius += strokeStyle().thickness / 2.0f;
            break;
        case StrokeStyle::Position::INSIDE:
            innerRadius = 0.0f;
            outerRadius += strokeStyle().thickness / 2.0f;
            break;
        case StrokeStyle::Position::OUTSIDE:
            innerRadius += strokeStyle().thickness / 2.0f;
            outerRadius = circle.radius * 2.0f;
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

        stroke(region, circle.center, Sdf::makeCircle({0, 0}, innerRadius, outerRadius));
    }

    void fill(Math::Circlei circle) {
        fill(circle.bound(), circle.center, Sdf::makeCircle({0, 0}, circle.radius));
    }

    Math::Vec2i mesure(Rune) {
        return {VGA_FONT_WIDTH, VGA_FONT_HEIGHT};
    }

    Math::Vec2i mesure(Str text) {
        int width = VGA_FONT_HEIGHT;
        for (auto rune : text.runes()) {
            width += mesure(rune).x;
        }
        return {width, VGA_FONT_HEIGHT};
    }

    void _draw(Math::Vec2i baseline, Rune rune, Color color) {
        One<Ibm437> one;
        encode_one<Ibm437>(rune, one);

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
        _draw(baseline, rune, strokeStyle().color);
    }

    void fill(Math::Vec2i baseline, Rune rune) {
        _draw(baseline, rune, fillStyle().color);
    }

    void stroke(Math::Vec2i baseline, Str text) {
        Math::Vec2i pos = baseline;
        for (auto rune : text.runes()) {
            stroke(pos, rune);
            baseline = baseline + Math::Vec2i{VGA_FONT_WIDTH, 0};
        }
    }

    void fill(Math::Vec2i baseline, Str text) {
        Math::Vec2i pos = baseline;
        for (auto rune : text.runes()) {
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
