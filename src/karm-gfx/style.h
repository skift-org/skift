#pragma once

namespace Karm::Gfx {

/* ---- Fill Style ---------------------------------------------------------- */

struct FillStyle {
    Color _color{};

    constexpr FillStyle() : _color(WHITE) {}

    constexpr FillStyle(Color color) : _color(color) {}

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

enum struct StrokePosition {
    CENTER,
    INSIDE,
    OUTSIDE,
};

using enum StrokePosition;

struct StrokeStyle {
    Color _color{};
    StrokePosition _position{};
    double _thickness{};

    constexpr StrokeStyle() : _color(WHITE), _thickness(1) {}

    constexpr StrokeStyle(Color color) : _color(color), _thickness(1) {}

    constexpr Color color() const { return _color; }

    constexpr StrokeStyle color(Color color) {
        _color = color;
        return *this;
    }

    constexpr StrokePosition position() const { return _position; }

    constexpr StrokeStyle position(StrokePosition position) {
        _position = position;
        return *this;
    }

    constexpr double thickness() const { return _thickness; }

    constexpr StrokeStyle thickness(double thickness) {
        _thickness = thickness;
        return *this;
    }
};

static inline constexpr StrokeStyle stroke(auto... args) {
    return StrokeStyle(args...);
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
    float _spread{};
    float _radius{};
    Math::Vec2f _offset{};

    constexpr ShadowStyle() : _color(BLACK), _spread(0.0f), _radius(0.0f), _offset(0.0f) {}

    constexpr ShadowStyle(Color paint) : _color(paint), _spread(0.0f), _radius(0.0f), _offset(0.0f) {}

    constexpr Color color() const { return _color; }

    constexpr ShadowStyle color(Color paint) {
        _color = paint;
        return *this;
    }

    constexpr float spread() const { return _spread; }

    constexpr ShadowStyle spread(float spread) {
        _spread = spread;
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
