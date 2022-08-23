#pragma once

#include <karm-math/vec.h>
#include <karm-media/font.h>

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

    constexpr auto &withColor(Color color) {
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

static inline constexpr StrokeStyle stroke(auto... args) {
    return {args...};
}

/* --- Text Style ----------------------------------------------------------- */

struct TextStyle {
    Strong<Media::Font> font = Media::Font::fallback();
    int size = 12;

    TextStyle() = default;

    TextStyle(Strong<Media::Font> f, int s = 12)
        : font(f), size(s) {}

    auto &withSize(int s) {
        size = s;
        return *this;
    }

    auto &withFont(Strong<Media::Font> f) {
        font = f;
        return *this;
    }
};

static inline TextStyle text(auto... args) {
    return TextStyle(args...);
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

static inline ShadowStyle shadow(auto... args) {
    return ShadowStyle(args...);
}

} // namespace Karm::Gfx
