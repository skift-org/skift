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
        return topLeft == 0 and topRight == 0 and bottomRight == 0 and bottomLeft == 0;
    }

    BorderRadius() = default;

    BorderRadius(double radius)
        : topLeft(radius), topRight(radius), bottomRight(radius), bottomLeft(radius) {}

    BorderRadius(double topLeft, double topRight, double bottomRight, double bottomLeft)
        : topLeft(topLeft), topRight(topRight), bottomRight(bottomRight), bottomLeft(bottomLeft) {}
};

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
    double radius{};
    Math::Vec2f offset{};

    constexpr ShadowStyle(Color c = BLACK) : color(c) {}

    constexpr auto &withColor(Color c) {
        color = c;
        return *this;
    }

    constexpr auto &withRadius(double r) {
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
