#pragma once

#include <karm-gfx/colors.h>
#include <karm-io/emit.h>
#include <karm-logger/logger.h>

#include "keywords.h"
#include "percent.h"

// https://www.w3.org/TR/css-color-4

namespace Vaev {

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

    void repr(Io::Emit& e) const {
        if (type == ColorSpace::RECTANGULAR) {
            e("{}", rectangular);
        } else {
            e("{} {}", polar, interpolation);
        }
    }

    static ColorSpace fromStr(Str name) {
        if (name == "lch")
            return ColorSpace(HSL, SHORTER);

        return ColorSpace();
    }

    template <typename Polar>
    void _preparePolar(Polar& a, Polar& b) const {
        auto delta = b.hue - a.hue;
        if (interpolation == SHORTER) {
            if (delta > 180.f)
                a.hue += 360.0f;
            else if (delta < -180.f)
                b.hue += 360.0f;
        } else if (interpolation == LONGER) {
            if (0.0f < delta and delta < 180.0f)
                a.hue += 360.0f;
            else if (-180.0f < delta and delta <= 0.0f)
                b.hue += 360.0f;
        } else if (interpolation == INCREASING) {
            if (delta < 0.0f)
                b.hue += 360.0f;
        } else {
            if (delta > 0.0f)
                a.hue += 360.0f;
        }
    }

    template <typename Polar>
    Polar _interpolatePolar(Polar a, Polar b, f64 t) const {
        _preparePolar(a, b);
        return a.lerpWith(b, t);
    }

    // https://drafts.csswg.org/css-color-4/#interpolation
    // Not compliant regarding missing or carried values
    Gfx::Color interpolate(Gfx::Color a, Gfx::Color b, f64 t) const {
        a = a.premultiply();
        b = b.premultiply();

        if (type == _Type::RECTANGULAR) {
            switch (rectangular) {
            case _Rectangular::SRGB:
                return a.lerpWith(b, t).unpremultiply();

            default:
                logWarn("interpolation method is not supported: {}", rectangular);
                return Gfx::FUCHSIA;
            }
        } else if (type == _Type::POLAR) {
            switch (polar) {
            case _Polar::HSL:
                return Gfx::hslToRgb(
                           _interpolatePolar<Gfx::Hsl>(
                               Gfx::rgbToHsl(a),
                               Gfx::rgbToHsl(b),
                               t
                           )
                )
                    .unpremultiply();

            default:
                logWarn("interpolation method is not supported: {}", polar);
                return Gfx::FUCHSIA;
            }
        }

        return Gfx::WHITE;
    }
};

// MARK: Color -----------------------------------------------------------------

enum struct SystemColor : u8 {
#define COLOR(NAME, ...) NAME,
#include "defs/system-colors.inc"

#undef COLOR

    _LEN
};

struct ColorMix;

using Color = Union<
    Gfx::Color,
    Keywords::CurrentColor,
    SystemColor,
    Box<ColorMix>>;

struct ColorMix {
    struct Side {
        Color color;
        Opt<Percent> perc;

        void repr(Io::Emit& e) const {
            e("{}", color);
            if (perc)
                e(" {}", *perc);
        }
    };

    ColorSpace colorSpace;
    Side lhs;
    Side rhs;

    void repr(Io::Emit& e) const {
        e("(color-mix {} {} {})", colorSpace, lhs, rhs);
    }
};

inline constexpr Gfx::Color TRANSPARENT = Gfx::Color::fromRgba(0, 0, 0, 0);

#define COLOR(NAME, _, VALUE) \
    inline constexpr Gfx::Color NAME = Gfx::Color::fromHex(VALUE);
#include "defs/colors.inc"
#undef COLOR

Opt<Color> parseNamedColor(Str name);

Opt<SystemColor> parseSystemColor(Str name);

Gfx::Color resolve(Color const& c, Gfx::Color currentColor);

} // namespace Vaev
