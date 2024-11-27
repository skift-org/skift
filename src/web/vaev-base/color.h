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

inline constexpr Gfx::Color TRANSPARENT = Gfx::Color::fromRgba(0, 0, 0, 0);

#define COLOR(NAME, _, VALUE) \
    inline constexpr Gfx::Color NAME = Gfx::Color::fromHex(VALUE);
#include "defs/colors.inc"
#undef COLOR

Opt<Color> parseNamedColor(Str name);

Opt<SystemColor> parseSystemColor(Str name);

Gfx::Color resolve(Color c, Gfx::Color currentColor);

// MARK: Color Interpolation ---------------------------------------------------

// https://www.w3.org/TR/css-color-4/#typedef-color-space
struct ColorSpace {
    enum struct _Type {
        POLAR,
        RECTANGULAR,

        _LEN0,
    };

    using enum _Type;

    // https://www.w3.org/TR/css-color-4/#typedef-rectangular-color-space
    enum struct _Rectangular {
        SRGB,
        SRGB_LINEAR,
        DISPLAY_P3,
        A98_RGB,
        PROPHOTO_RGB,
        REC2020,
        LAB,
        OKLAB,
        XYZ,
        XYZ_D50,
        XYZ_D65,

        _LEN1,

    };

    using enum _Rectangular;

    // https://www.w3.org/TR/css-color-4/#typedef-hue-interpolation-method
    enum struct _Interpolation {
        SHORTER,
        LONGER,
        INCREASING,
        DECREASING,

        _LEN2,
    };

    using enum _Interpolation;

    // https://www.w3.org/TR/css-color-4/#typedef-polar-color-space
    enum struct _Polar {
        HSL,
        HWB,
        LCH,
        OKLCH,

        _LEN3,
    };

    using enum _Polar;

    _Type type;

    union {
        _Rectangular rectangular;

        struct {
            _Polar polar;
            _Interpolation interpolation;
        };
    };

    constexpr ColorSpace(_Rectangular rectangular)
        : type(_Type::RECTANGULAR), rectangular(rectangular) {
    }

    constexpr ColorSpace(_Polar polar, _Interpolation interpolation)
        : type(_Type::POLAR), polar(polar), interpolation(interpolation) {
    }

    constexpr ColorSpace()
        : type(_Type::RECTANGULAR), rectangular(_Rectangular::SRGB) {
    }

    void repr(Io::Emit &e) const {
        if (type == ColorSpace::RECTANGULAR) {
            e("{}", rectangular);
        } else {
            e("{} {}", polar, interpolation);
        }
    }
};

} // namespace Vaev
