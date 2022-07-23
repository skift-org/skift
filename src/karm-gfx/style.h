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

struct StrokeStyle {
    enum struct Align {
        CENTER,
        INSIDE,
        OUTSIDE,
    };

    enum struct Cap {
        BUTT,
        SQUARE,
        ROUND,
    };

    enum struct Join {
        BEVEL,
        MITER,
        ROUND,
    };

    Color color{};
    double width{1};
    Align align{};
    Cap cap{};
    Join join{};

    constexpr StrokeStyle()
        : color(WHITE) {}

    constexpr StrokeStyle(Color color)
        : color(color) {}

    constexpr auto with(Color c) {
        color = c;
        return *this;
    }

    constexpr auto with(double w) {
        width = w;
        return *this;
    }

    constexpr auto with(Align a) {
        align = a;
        return *this;
    }

    constexpr auto with(Cap c) {
        cap = c;
        return *this;
    }

    constexpr auto with(Join j) {
        join = j;
        return *this;
    }
};

static inline constexpr StrokeStyle stroke(auto... args) {
    return {args...};
}

/* --- Text Style ----------------------------------------------------------- */

struct TextStyle {
    Color _color{};

    constexpr TextStyle() : _color(WHITE) {}

    constexpr TextStyle(Color paint) : _color(paint) {}

    constexpr Color color() const { return _color; }

    constexpr TextStyle color(Color paint) {
        _color = paint;
        return *this;
    }
};

static inline TextStyle text(auto... args) {
    return TextStyle(args...);
}

/* --- Shadow Style --------------------------------------------------------- */

struct ShadowStyle {
    Color _color{};
    float _radius{};
    Math::Vec2f _offset{};

    constexpr ShadowStyle() : _color(BLACK) {}

    constexpr ShadowStyle(Color paint) : _color(paint) {}

    constexpr Color color() const { return _color; }

    constexpr ShadowStyle color(Color paint) {
        _color = paint;
        return *this;
    }

    constexpr float radius() const { return _radius; }

    constexpr ShadowStyle radius(float radius) {
        _radius = radius;
        return *this;
    }

    constexpr Math::Vec2f offset() const { return _offset; }

    constexpr ShadowStyle offset(Math::Vec2f offset) {
        _offset = offset;
        return *this;
    }
};

static inline ShadowStyle shadow(auto... args) {
    return ShadowStyle(args...);
}

} // namespace Karm::Gfx
