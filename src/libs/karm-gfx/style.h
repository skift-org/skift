#pragma once

#include <karm-math/vec.h>
#include <karm-media/font.h>

#include "paint.h"

namespace Karm::Gfx {

/* --- Border Style -------------------------------------------------------- */

struct BorderRadius {
    f64 topLeft{};
    f64 topRight{};
    f64 bottomRight{};
    f64 bottomLeft{};

    bool zero() const {
        return topLeft == 0 and topRight == 0 and bottomRight == 0 and bottomLeft == 0;
    }

    BorderRadius() = default;

    BorderRadius(f64 radius)
        : topLeft(radius), topRight(radius), bottomRight(radius), bottomLeft(radius) {}

    BorderRadius(f64 topLeft, f64 topRight, f64 bottomRight, f64 bottomLeft)
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
    Paint paint;
    f64 width{1};
    StrokeAlign align{};
    StrokeCap cap{};
    StrokeJoin join{};

    StrokeStyle(Paint c = WHITE) : paint(c) {}

    auto &withPaint(Paint p) {
        paint = p;
        return *this;
    }

    auto &withWidth(f64 w) {
        width = w;
        return *this;
    }

    auto &withAlign(StrokeAlign a) {
        align = a;
        return *this;
    }

    auto &withCap(StrokeCap c) {
        cap = c;
        return *this;
    }

    auto &withJoin(StrokeJoin j) {
        join = j;
        return *this;
    }
};

inline StrokeStyle stroke(auto... args) {
    return {args...};
}

/* --- Shadow Style --------------------------------------------------------- */

struct ShadowStyle {
    Paint paint;
    f64 radius{};
    Math::Vec2f offset{};

    ShadowStyle(Paint c = BLACK) : paint(c) {}

    auto &withPaint(Paint p) {
        paint = p;
        return *this;
    }

    auto &withRadius(f64 r) {
        radius = r;
        return *this;
    }

    auto &withOffset(Math::Vec2f o) {
        offset = o;
        return *this;
    }
};

inline ShadowStyle shadow(auto... args) {
    return ShadowStyle(args...);
}

} // namespace Karm::Gfx
