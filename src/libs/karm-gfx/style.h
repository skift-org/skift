#pragma once

#include <karm-math/vec.h>
#include <karm-media/font.h>

#include "colors.h"

namespace Karm::Gfx {

/* --- Border Style -------------------------------------------------------- */

struct BorderRadius {
    double topLeft{};
    double topRight{};
    double bottomRight{};
    double bottomLeft{};

    bool zero() const {
        return topLeft == 0 && topRight == 0 && bottomRight == 0 && bottomLeft == 0;
    }

    BorderRadius() = default;

    BorderRadius(float radius)
        : topLeft(radius), topRight(radius), bottomRight(radius), bottomLeft(radius) {}

    BorderRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)
        : topLeft(topLeft), topRight(topRight), bottomRight(bottomRight), bottomLeft(bottomLeft) {}
};

/* ---- Fill Style ---------------------------------------------------------- */

enum struct FillRule {
    NONZERO,
    EVENODD,
};

struct FillStyle {
    Color color{};
    FillRule rule = FillRule::NONZERO;

    constexpr FillStyle()
        : color(WHITE) {}

    constexpr FillStyle(Color color, FillRule rule = FillRule::NONZERO)
        : color(color), rule(rule) {}

    constexpr auto &withColor(Color color) {
        this->color = color;
        return *this;
    }

    constexpr auto &withRule(FillRule rule) {
        this->rule = rule;
        return *this;
    }
};

inline constexpr FillStyle fill(auto... args) {
    return FillStyle(args...);
}

/* --- Stroke Style --------------------------------------------------------- */

enum StrokeAlign {
    CENTER_ALIGN,
    INSIDE_ALIGN,
    OUTSIDE_ALIGN,
};

enum StrokeCap {
    BUTT_CAP,
    SQUARE_CAP,
    ROUND_CAP,
};

enum StrokeJoin {
    BEVEL_JOIN,
    MITER_JOIN,
    ROUND_JOIN,
};

struct StrokeStyle {
    Color color{};
    double width{1};
    StrokeAlign align{};
    StrokeCap cap{};
    StrokeJoin join{};

    constexpr StrokeStyle(Color c = WHITE) : color(c) {}

    constexpr auto &withColor(Color c) {
        color = c;
        return *this;
    }

    constexpr auto &withWidth(double w) {
        width = w;
        return *this;
    }

    constexpr auto &withAlign(StrokeAlign a) {
        align = a;
        return *this;
    }

    constexpr auto &withCap(StrokeCap c) {
        cap = c;
        return *this;
    }

    constexpr auto &withJoin(StrokeJoin j) {
        join = j;
        return *this;
    }
};

inline constexpr StrokeStyle stroke(auto... args) {
    return {args...};
}

/* --- Shadow Style --------------------------------------------------------- */

struct ShadowStyle {
    Color color{};
    float radius{};
    Math::Vec2f offset{};

    constexpr ShadowStyle(Color c = BLACK) : color(c) {}

    constexpr auto &withColor(Color c) {
        color = c;
        return *this;
    }

    constexpr auto &withRadius(float r) {
        radius = r;
        return *this;
    }

    constexpr auto &withOffset(Math::Vec2f o) {
        offset = o;
        return *this;
    }
};

inline ShadowStyle shadow(auto... args) {
    return ShadowStyle(args...);
}

} // namespace Karm::Gfx
