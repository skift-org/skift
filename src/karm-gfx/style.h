#pragma once

#include <karm-math/vec.h>

#include "colors.h"

namespace Karm::Gfx {

/* ---- Fill Style ---------------------------------------------------------- */

struct FillStyle {
    Color _color{};

    constexpr FillStyle()
        : _color(WHITE) {}

    constexpr FillStyle(Color color)
        : _color(color) {}

    constexpr Color color() const { return _color; }

    constexpr FillStyle color(Color color) {
        _color = color;
        return *this;
    }
};

static inline constexpr FillStyle fill(auto... args) {
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

struct Stroke {
    Color color{};
    double width{1};
    StrokeAlign align{};
    StrokeCap cap{};
    StrokeJoin join{};

    constexpr Stroke(Color c = WHITE) : color(c) {}

    constexpr auto withColor(Color c) {
        color = c;
        return *this;
    }

    constexpr auto withWidth(double w) {
        width = w;
        return *this;
    }

    constexpr auto withAlign(StrokeAlign a) {
        align = a;
        return *this;
    }

    constexpr auto withCap(StrokeCap c) {
        cap = c;
        return *this;
    }

    constexpr auto withJoin(StrokeJoin j) {
        join = j;
        return *this;
    }
};

static inline constexpr Stroke stroke(auto... args) {
    return {args...};
}

/* --- Text Style ----------------------------------------------------------- */

struct Text {
    Color color{WHITE};

    constexpr Text(Color c = WHITE) : color(c) {}

    constexpr Text withColor(Color c) {
        color = c;
        return *this;
    }
};

static inline Text text(auto... args) {
    return Text(args...);
}

/* --- Shadow Style --------------------------------------------------------- */

struct Shadow {
    Color color{};
    float radius{};
    Math::Vec2f offset{};

    constexpr Shadow(Color c = BLACK) : color(c) {}

    constexpr Shadow withColor(Color c) {
        color = c;
        return *this;
    }

    constexpr Shadow withRadius(float r) {
        radius = r;
        return *this;
    }

    constexpr Shadow withOffset(Math::Vec2f o) {
        offset = o;
        return *this;
    }
};

static inline Shadow shadow(auto... args) {
    return Shadow(args...);
}

} // namespace Karm::Gfx
