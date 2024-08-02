#pragma once

#include <karm-gfx/colors.h>
#include <karm-io/emit.h>

// https://www.w3.org/TR/css-color-4

namespace Vaev {

enum struct SystemColor : u8 {
#define COLOR(NAME, ...) NAME,
#include "defs/system-colors.inc"
#undef COLOR

    _LEN
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

    constexpr Color()
        : type(Type::SRGB), srgb(Gfx::ALPHA) {}

    constexpr Color(Type type)
        : type(type) {}

    constexpr Color(Gfx::Color srgb)
        : type(Type::SRGB), srgb(srgb) {}

    constexpr Color(SystemColor system)
        : type(Type::SYSTEM), system(system) {}

    void repr(Io::Emit &e) const {
        switch (type) {
        case Type::SRGB:
            e("#{02x}{02x}{02x}{02x}", srgb.red, srgb.green, srgb.blue, srgb.alpha);
            break;

        case Type::SYSTEM:
            e("{}", system);
            break;

        case Type::CURRENT:
            e("currentColor");
            break;
        }
    }
};

struct ColorContext {
    Gfx::Color currentColor = Gfx::BLACK;
    Array<Gfx::Color, static_cast<usize>(SystemColor::_LEN)> systemColors = {
#define COLOR(NAME, _, VALUE) Gfx::Color::fromHex(VALUE),
#include "defs/system-colors.inc"
#undef COLOR
    };

    constexpr Gfx::Color resolve(Color const &c) const {
        switch (c.type) {
        case Color::Type::SRGB:
            return c.srgb;

        case Color::Type::SYSTEM:
            return systemColors[static_cast<usize>(c.system)];

        case Color::Type::CURRENT:
            return currentColor;
        }
    }
};

inline constexpr Gfx::Color TRANSPARENT = Gfx::Color::fromRgba(0, 0, 0, 0);

#define COLOR(NAME, _, VALUE) \
    inline constexpr Gfx::Color NAME = Gfx::Color::fromHex(VALUE);
#include "defs/colors.inc"
#undef COLOR

Opt<Color> parseNamedColor(Str name);

Opt<SystemColor> parseSystemColor(Str name);

} // namespace Vaev
