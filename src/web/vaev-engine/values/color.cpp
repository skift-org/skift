module;

#include <karm-gfx/colors.h>
#include <karm-logger/logger.h>

export module Vaev.Engine:values.color;

import Karm.Core;

import :css;
import :values.base;
import :values.angle;
import :values.keywords;
import :values.percent;

using namespace Karm;

// https://www.w3.org/TR/css-color-4

namespace Vaev {

// MARK: Color Interpolation ---------------------------------------------------

// https://www.w3.org/TR/css-color-4/#typedef-color-space
export struct ColorSpace {
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
// https://drafts.csswg.org/css-color

export enum struct SystemColor : u8 {
#define COLOR(NAME, ...) NAME,
#include "defs/system-colors.inc"

#undef COLOR

    _LEN
};

export struct ColorMix;

export using Color = Union<
    Gfx::Color,
    Keywords::CurrentColor,
    SystemColor,
    Box<ColorMix>>;

export struct ColorMix {
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

export constexpr Gfx::Color TRANSPARENT = Gfx::Color::fromRgba(0, 0, 0, 0);

#define COLOR(NAME, _, VALUE) \
    export constexpr Gfx::Color NAME = Gfx::Color::fromHex(VALUE);
#include "defs/colors.inc"
#undef COLOR

export Opt<Color> parseNamedColor(Str name) {

#define COLOR(ID, NAME, ...)   \
    if (eqCi(name, #NAME ""s)) \
        return ID;
#include "defs/colors.inc"

#undef COLOR

    return NONE;
}

export Opt<SystemColor> parseSystemColor(Str name) {
#define COLOR(ID, NAME, ...) \
    if (name == #NAME)       \
        return SystemColor::ID;
#include "defs/system-colors.inc"

#undef COLOR

    return NONE;
}

export template <>
struct ValueParser<Color> {

    static Res<Gfx::Color> _parseHexColor(Io::SScan& s) {
        if (s.next() != '#')
            panic("expected '#'");

        auto nextHex = [&](usize len) {
            return Io::atou(s.slice(len), {.base = 16}).unwrapOr(0);
        };

        if (s.rem() == 3) {
            // #RGB
            auto r = nextHex(1);
            auto g = nextHex(1);
            auto b = nextHex(1);
            return Ok(Gfx::Color::fromRgb(r | (r << 4), g | (g << 4), b | (b << 4)));
        } else if (s.rem() == 4) {
            // #RGBA
            auto r = nextHex(1);
            auto g = nextHex(1);
            auto b = nextHex(1);
            auto a = nextHex(1);
            return Ok(Gfx::Color::fromRgba(r | (r << 4), g | (g << 4), b | (b << 4), a | (a << 4)));
        } else if (s.rem() == 6) {
            // #RRGGBB
            auto r = nextHex(2);
            auto g = nextHex(2);
            auto b = nextHex(2);
            return Ok(Gfx::Color::fromRgb(r, g, b));
        } else if (s.rem() == 8) {
            // #RRGGBBAA
            auto r = nextHex(2);
            auto g = nextHex(2);
            auto b = nextHex(2);
            auto a = nextHex(2);
            return Ok(Gfx::Color::fromRgba(r, g, b, a));
        } else {
            return Error::invalidData("invalid color format");
        }
    }

    static Res<u8> _parseAlphaValue(Cursor<Css::Sst>& scan) {
        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        if (scan.peek() == Css::Token::Type::PERCENTAGE) {
            auto perc = try$(parseValue<Percent>(scan)).value();
            return Ok(Math::round((perc * 255) / 100));
        } else if (scan.peek() == Css::Token::Type::NUMBER)
            return Ok(Math::round(try$(parseValue<Number>(scan)) * 255));
        return Error::invalidData("expected alpha channel");
    }

    static Res<Opt<u8>> _parseAlphaComponentModernSyntax(Cursor<Css::Sst>& scan) {
        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        if (not scan.skip(Css::Token::delim("/")))
            return Error::invalidData("expected '/' before alpha channel");

        eatWhitespace(scan);
        if (scan.skip(Css::Token::ident("none")))
            return Ok(NONE);

        return Ok(try$(_parseAlphaValue(scan)));
    }

    static Res<Number> _parseNumPercNone(Cursor<Css::Sst>& scan, Number percOf, Number noneValue = 0) {
        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        if (scan.skip(Css::Token::ident("none"))) {
            return Ok(noneValue);
        } else if (scan.peek() == Css::Token::Type::PERCENTAGE) {
            auto perc = try$(parseValue<Percent>(scan));
            return Ok((perc.value() * percOf) / 100.0f);
        } else
            return Ok(try$(parseValue<Number>(scan)));
    }

    static Res<Array<Number, 3>> _parsePredefinedRGBParams(Cursor<Css::Sst>& scan, Number percOf = 255) {
        Array<Number, 3> channels{};
        for (usize i = 0; i < 3; ++i) {

            if (scan.ended())
                return Error::invalidData("unexpected end of input");

            channels[i] = Math::round(try$(_parseNumPercNone(scan, percOf)));

            eatWhitespace(scan);
        }
        return Ok(channels);
    }

    // https://drafts.csswg.org/css-color/#rgb-functions
    static Res<Gfx::Color> _parseSRGBModernFuncColor(Css::Sst const& s) {
        Cursor<Css::Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        auto channels = try$(_parsePredefinedRGBParams(scan));
        u8 r = Math::round(channels[0]);
        u8 g = Math::round(channels[1]);
        u8 b = Math::round(channels[2]);

        eatWhitespace(scan);

        if (scan.ended())
            return Ok(Gfx::Color::fromRgb(r, g, b));

        auto alphaComponent = try$(_parseAlphaComponentModernSyntax(scan));
        return Ok(Gfx::Color::fromRgba(r, g, b, alphaComponent.unwrapOr(0)));
    }

    static Res<Gfx::Color> _parseSRGBLegacyFuncColor(Css::Sst const& s) {
        Cursor<Css::Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        bool usingPercentages = scan.peek() == Css::Token::Type::PERCENTAGE;

        Array<u8, 3> channels{};
        for (usize i = 0; i < 3; ++i) {
            if (scan.ended())
                return Error::invalidData("unexpected end of input");

            if (usingPercentages) {
                auto perc = try$(parseValue<Percent>(scan));
                channels[i] = Math::round((perc.value() * 255) / 100.0f);
            } else {
                channels[i] = try$(parseValue<Number>(scan));
            }
            eatWhitespace(scan);
            if (i < 2) {
                if (not scan.skip(Css::Token::COMMA))
                    return Error::invalidData("expected comma between rgb color channels");
                eatWhitespace(scan);
            }
        }

        u8 r = channels[0];
        u8 g = channels[1];
        u8 b = channels[2];

        eatWhitespace(scan);
        if (scan.ended())
            return Ok(Gfx::Color::fromRgb(r, g, b));

        if (not scan.skip(Css::Token::COMMA))
            return Error::invalidData("expected comma before alpha channel");

        eatWhitespace(scan);
        return Ok(Gfx::Color::fromRgba(r, g, b, try$(_parseAlphaValue(scan))));
    }

    // https://drafts.csswg.org/css-color/#funcdef-hsl
    static Res<Gfx::Color> _parseHSLModernFuncColor(Css::Sst const& s) {
        Cursor<Css::Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        Number hue;
        if (scan.skip(Css::Token::ident("none")))
            hue = 0;
        else if (scan.peek() == Css::Token::DIMENSION)
            hue = try$(parseValue<Angle>(scan)).toDegree();
        else if (scan.peek() == Css::Token::Type::NUMBER)
            hue = try$(parseValue<Number>(scan));
        else
            return Error::invalidData("expected hue");

        eatWhitespace(scan);

        auto sat = try$(_parseNumPercNone(scan, 1));
        eatWhitespace(scan);

        auto l = try$(_parseNumPercNone(scan, 1));
        eatWhitespace(scan);

        if (scan.ended())
            return Ok(Gfx::hslToRgb(Gfx::Hsl{hue, sat, l}));

        auto alpha = try$(_parseAlphaComponentModernSyntax(scan))
                         .unwrapOr(255);
        auto color = Gfx::hslToRgb(Gfx::Hsl{hue, sat, l});
        return Ok(color.withAlpha(alpha));
    }

    static Res<Gfx::Color> _parseHSLLegacyFuncColor(Css::Sst const& s) {
        Cursor<Css::Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        Number hue;
        if (scan.skip(Css::Token::ident("none")))
            hue = 0;
        else if (scan.peek() == Css::Token::DIMENSION)
            hue = try$(parseValue<Angle>(scan)).toDegree();
        else if (scan.peek() == Css::Token::Type::NUMBER)
            hue = try$(parseValue<Number>(scan));
        else
            return Error::invalidData("expected hue");

        eatWhitespace(scan);

        if (not scan.skip(Css::Token::COMMA))
            return Error::invalidData("comma is expected separating hsl components");
        eatWhitespace(scan);

        auto sat = try$(parseValue<Percent>(scan)).value() / 100.0f;
        eatWhitespace(scan);

        if (not scan.skip(Css::Token::COMMA))
            return Error::invalidData("comma is expected separating hsl components");
        eatWhitespace(scan);

        auto l = try$(parseValue<Percent>(scan)).value() / 100.0f;
        eatWhitespace(scan);

        if (scan.ended()) {
            auto color = Gfx::hslToRgb(Gfx::Hsl{hue, sat, l});
            return Ok(color);
        }

        if (not scan.skip(Css::Token::COMMA))
            return Error::invalidData("expected comma before alpha channel");

        eatWhitespace(scan);
        auto alpha = try$(_parseAlphaValue(scan));
        auto color = Gfx::hslToRgb(Gfx::Hsl{hue, sat, l});
        return Ok(color.withAlpha(alpha));
    }

    // https://drafts.csswg.org/css-color-4/#predefined
    static Res<Gfx::Color> _parseColorFuncColor(Css::Sst const& s) {
        Cursor<Css::Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended() or not(scan.peek() == Css::Token::IDENT))
            return Error::invalidData("expected color-space identifier");

        auto colorSpace = scan.next().token.data;
        eatWhitespace(scan);

        // FIXME: predefined-rgb
        if (
            colorSpace == "srgb" or colorSpace == "srgb-linear" or
            colorSpace == "display-p3" or colorSpace == "a98-rgb" or
            colorSpace == "prophoto-rgb" or colorSpace == "rec2020"
        ) {
            auto channels = try$(_parsePredefinedRGBParams(scan, 1));

            Number r = channels[0];
            Number g = channels[1];
            Number b = channels[2];

            eatWhitespace(scan);

            if (scan.ended()) {
                // FIXME: dispatch to constructor of colorSpace without alpha
                return Ok(
                    Gfx::Color::fromRgb(
                        Math::round(r * 255),
                        Math::round(g * 255),
                        Math::round(b * 255)
                    )
                );
            }

            if (auto alphaComponent = try$(_parseAlphaComponentModernSyntax(scan))) {
                // FIXME: dispatch to constructor of colorSpace with alpha
                return Ok(
                    Gfx::Color::fromRgba(
                        Math::round(r * 255),
                        Math::round(g * 255),
                        Math::round(b * 255),
                        alphaComponent.unwrap()
                    )
                );
            } else {
                // FIXME: correctly deal with missing alpha values
                // FIXME: dispatch to constructor of colorSpace with missing alpha
                return Ok(
                    Gfx::Color::fromRgba(
                        Math::round(r * 255),
                        Math::round(g * 255),
                        Math::round(b * 255),
                        0
                    )
                );
            }
        } else {
            logWarn("predefined color space not implemented: {}", colorSpace);
            return Ok(Gfx::WHITE);
        }
    }

    static Res<Gfx::Color> _parseFuncColor(Css::Sst const& s) {
        if (s.prefix == Css::Token::function("color(")) {
            return _parseColorFuncColor(s);
        } else if (s.prefix == Css::Token::function("rgb(") or s.prefix == Css::Token::function("rgba(")) {
            auto modernColor = _parseSRGBModernFuncColor(s);
            if (modernColor)
                return modernColor;

            return _parseSRGBLegacyFuncColor(s);
        } else if (s.prefix == Css::Token::function("hsl(") or s.prefix == Css::Token::function("hsla(")) {
            auto modernColor = _parseHSLModernFuncColor(s);
            if (modernColor)
                return modernColor;

            return _parseHSLLegacyFuncColor(s);
        } else {
            return Error::invalidData("unknown color function");
        }
    }

    static Res<ColorMix::Side> _parseColorMixSide(Cursor<Css::Sst>& s) {
        Opt<Percent> percent;

        if (s.ended())
            return Error::invalidData("unexpected end of input");

        if (s.peek() == Css::Token::Type::PERCENTAGE) {
            percent = try$(parseValue<Percent>(s));
            eatWhitespace(s);
            return Ok(ColorMix::Side{
                try$(parseValue<Color>(s)),
                percent,
            });
        }

        Color color = try$(parseValue<Color>(s));
        eatWhitespace(s);
        if (not s.ended() and s.peek() == Css::Token::Type::PERCENTAGE)
            percent = try$(parseValue<Percent>(s)).value();

        return Ok(ColorMix::Side{
            std::move(color),
            percent,
        });
    }

    static Res<Color> _parseColorMixFunc(Css::Sst const& s) {
        Cursor<Css::Sst> scan = s.content;

        eatWhitespace(scan);
        if (not scan.skip(Css::Token::ident("in"))) // FIXME: correct token class?
            return Error::invalidData("expected 'in' keyword");

        eatWhitespace(scan);
        if (scan.ended() or not(scan.peek() == Css::Token::IDENT))
            return Error::invalidData("expected color-space identifier");

        // FIXME: not parsing interpolation method for polar spaces
        auto colorSpace = scan.next().token.data;
        if (not skipOmmitableComma(scan))
            return Error::invalidData("expected comma separting color mix arguments");

        auto lhs = try$(_parseColorMixSide(scan));
        if (not skipOmmitableComma(scan))
            return Error::invalidData("expected comma");

        auto rhs = try$(_parseColorMixSide(scan));
        return Ok(makeBox<ColorMix>(ColorSpace::fromStr(colorSpace), std::move(lhs), std::move(rhs)));
    }

    static Res<Color> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::HASH) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(try$(_parseHexColor(scan)));
        } else if (c.peek() == Css::Token::IDENT) {
            Str data = c->token.data.str();

            if (eqCi(data, "currentcolor"s)) {
                c.next();
                return Ok(Keywords::CURRENT_COLOR);
            }

            if (eqCi(data, "transparent"s)) {
                c.next();
                return Ok(TRANSPARENT);
            }

            auto maybeColor = parseNamedColor(data);
            if (maybeColor) {
                c.next();
                return Ok(maybeColor.unwrap());
            }

            auto maybeSystemColor = parseSystemColor(data);
            if (maybeSystemColor) {
                c.next();
                return Ok(maybeSystemColor.unwrap());
            }

        } else if (c.peek() == Css::Sst::FUNC) {

            if (c->prefix == Css::Token::function("color-mix(")) {
                auto color = try$(_parseColorMixFunc(*c));
                c.next();
                return Ok(color);
            }

            auto color = try$(_parseFuncColor(*c));
            c.next();
            return Ok(color);
        }

        return Error::invalidData("expected color");
    }
};

static Array<Gfx::Color, static_cast<usize>(SystemColor::_LEN)> SYSTEM_COLOR = {
#define COLOR(NAME, _, VALUE) Gfx::Color::fromHex(VALUE),
#include "defs/system-colors.inc"

#undef COLOR
};

export Gfx::Color resolve(ColorMix const& cm, Gfx::Color currentColor);

export Gfx::Color resolve(Color const& c, Gfx::Color currentColor) {
    return c.visit(Visitor{
        [&](Gfx::Color const& srgb) {
            return srgb;
        },
        [&](Keywords::CurrentColor) {
            return currentColor;
        },
        [&](SystemColor const& system) {
            return SYSTEM_COLOR[static_cast<usize>(system)];
        },
        [&](ColorMix const& mix) {
            return resolve(mix, currentColor);
        },
    });
}

export Gfx::Color resolve(ColorMix const& cm, Gfx::Color currentColor) {
    Gfx::Color lhsColor = resolve(cm.lhs.color, currentColor);
    Percent lhsPerc = cm.lhs.perc.unwrapOrElse([&] {
        return Percent{100} - cm.rhs.perc.unwrapOr(Percent{50});
    });

    Gfx::Color rhsColor = resolve(cm.rhs.color, currentColor);
    Percent rhsPerc = cm.rhs.perc.unwrapOrElse([&] {
        return Percent{100} - cm.lhs.perc.unwrapOr(Percent{50});
    });

    if (lhsPerc == rhsPerc and lhsPerc == Percent{0}) {
        logWarn("cannot mix colors when both have zero percentages");
        return Gfx::WHITE;
    }

    Percent rhsPercNorm = rhsPerc;

    if (lhsPerc + rhsPerc != Percent{100})
        rhsPercNorm = rhsPercNorm / (lhsPerc + rhsPerc);
    else
        rhsPercNorm /= Percent{100};

    Gfx::Color resColor = cm.colorSpace.interpolate(lhsColor, rhsColor, rhsPercNorm.value());
    if (lhsPerc + rhsPerc < Percent{100})
        resColor = resColor.withOpacity((lhsPerc + rhsPerc).value() / 100.0);

    return resColor;
}

} // namespace Vaev
