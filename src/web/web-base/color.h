#pragma once

#include <karm-gfx/colors.h>

// https://www.w3.org/TR/css-color-4

namespace Web {

enum struct SystemColor {
#define COLOR(NAME, ...) NAME,
#include "defs/system-colors.inc"
#undef COLOR
};

struct Color {
    enum struct Type {
        SRGB,
        SYSTEM,
        CURRENT, // currentColor
    };

    using enum Type;

    Type type;
    union {
        Gfx::Color srgb;
        SystemColor system;
    };

    Color()
        : type(Type::SRGB), srgb(Gfx::ALPHA) {}

    Color(Type type)
        : type(type) {}

    Color(Gfx::Color srgb)
        : type(Type::SRGB), srgb(srgb) {}

    Color(SystemColor system)
        : type(Type::SYSTEM), system(system) {}
};

inline constexpr Gfx::Color TRANSPARENT = Gfx::Color::fromRgba(0, 0, 0, 0);

#define COLOR(NAME, _, VALUE) \
    inline constexpr Gfx::Color NAME = Gfx::Color::fromHex(VALUE);
#include "defs/colors.inc"
#undef COLOR

Opt<Color> parseNamedColor(Str name);

Opt<SystemColor> parseSystemColor(Str name);

} // namespace Web
