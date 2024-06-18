#include "builder2.h"

namespace Web::Css {

// MARK: Types -----------------------------------------------------------------

// NOTE: Please keep this alphabetically sorted.

// MARK: Color
// https://drafts.csswg.org/css-color

static Res<Gfx::Color> _parseHexColor(Io::SScan &s) {
    if (s.next() != '#')
        panic("expected '#'");

    auto nextHex = [&](usize len) {
        return tryOr(Io::atou(s.slice(len), {.base = 16}), 0);
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
        return Ok(Gfx::Color::fromRgba(r, g, b, a));
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

Res<Color> parseColor(Cursor<Sst> &c) {
    if (c.peek() == Token::HASH) {
        Io::SScan scan = c->token.data;
        return Ok(try$(_parseHexColor(scan)));
    } else if (c.peek() == Token::IDENT) {
        Str data = c->token.data;
        c.next();

        auto maybeColor = parseNamedColor(data);
        if (maybeColor)
            return Ok(maybeColor.unwrap());

        auto maybeSystemColor = parseSystemColor(data);
        if (maybeSystemColor)
            return Ok(maybeSystemColor.unwrap());

        if (data == "currentColor")
            return Ok(Color::CURRENT);

        if (data == "transparent")
            return Ok(TRANSPARENT);

        return Error::invalidData("unknown color");
    } else {
        return Error::invalidData("expected color");
    }
}

// MARK: Length
// https://drafts.csswg.org/css-values/#lengths

static Res<Length::Unit> _parseLengthUnit(Str unit) {
#define LENGTH(NAME, ...)      \
    if (eqCi(unit, #NAME ""s)) \
        return Ok(Length::Unit::NAME);
#include <web-base/defs/lengths.inc>
#undef LENGTH

    return Error::invalidData("unknown length unit");
}

Res<Length> parseLength(Cursor<Sst> &c) {
    if (c.peek() == Token::DIMENSION) {
        Io::SScan scan = c->token.data;
        auto value = tryOr(Io::atof(scan), 0.0);
        auto unit = try$(_parseLengthUnit(scan.remStr()));
        return Ok(Length{value, unit});
    } else {
        return Error::invalidData("expected length");
    }
}

// MARK: Size
// https://drafts.csswg.org/css-sizing-4/#sizing-values

Res<Size> parseSize(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("expected size");

    auto sst = c.next();

    if (sst == Token::IDENT) {
        auto data = sst.token.data;
        if (data == "auto") {
            return Ok(Size::AUTO);
        } else if (data == "none") {
            return Ok(Size::NONE);
        } else if (data == "min-content") {
            return Ok(Size::MIN_CONTENT);
        } else if (data == "max-content") {
            return Ok(Size::MAX_CONTENT);
        }
    } else if (sst == Token::PERCENTAGE) {
        return Ok(try$(parsePercentage(c)));
    } else if (sst == Token::DIMENSION) {
        return Ok(try$(parseLength(c)));
    } else if (sst == Sst::FUNC) {
        auto const &prefix = sst.prefix.unwrap();
        auto prefixToken = prefix->token;
        if (prefixToken.data == "fit-content") {
            Cursor<Sst> content = prefix->content;
            return Ok(Size{Size::FIT_CONTENT, try$(parseLength(content))});
        }
    }

    return Error::invalidData("expected size");
}

// MARK: Percentage
// https://drafts.csswg.org/css-values/#percentages

Res<Percent> parsePercentage(Cursor<Sst> &c) {
    if (c.peek() == Token::PERCENTAGE) {
        Io::SScan scan = c->token.data;
        auto value = tryOr(Io::atof(scan), 0.0);
        if (scan.remStr() != "%")
            return Error::invalidData("invalid percentage");

        return Ok(Percent{value});
    } else {
        return Error::invalidData("expected percentage");
    }
}

Res<PercentOr<Length>> parseLengthOrPercentage(Cursor<Sst> &c) {
    if (c.peek() == Token::DIMENSION)
        return Ok(try$(parseLength(c)));
    else if (c.peek() == Token::PERCENTAGE)
        return Ok(try$(parsePercentage(c)));
    else
        return Error::invalidData("expected length or percentage");
}

// MARK: Media Queries ---------------------------------------------------------

Style::MediaQuery parseMediaQuery(Cursor<Sst> &) {
    return {};
}

// MARK: Selectors -------------------------------------------------------------

Style::Selector parseSelector(Cursor<Sst> &) {
    return {};
}

// MARK: Properties ------------------------------------------------------------

// NOTE: Please keep this alphabetically sorted.

Res<> _parseProp(Cursor<Sst> &c, Style::BackgroundColorProp &p) {
    while (not c.ended())
        p.value.pushBack(try$(parseColor(c)));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::ColorProp &p) {
    p.value = try$(parseColor(c));
    return Ok();
}

// MARK: Sizing

Res<> _parseProp(Cursor<Sst> &c, Style::WidthProp &p) {
    p.value = try$(parseLength(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::HeightProp &p) {
    p.value = try$(parseLength(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MinWidthProp &p) {
    p.value = try$(parseLength(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MinHeightProp &p) {
    p.value = try$(parseLength(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MaxWidthProp &p) {
    p.value = try$(parseLength(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MaxHeightProp &p) {
    p.value = try$(parseLength(c));
    return Ok();
}

// MARK: Properties Common

Res<Style::Prop> parseProperty(Sst const &sst) {
    if (sst != Sst::DECL)
        panic("expected declaration");

    if (sst.token != Token::IDENT)
        panic("expected ident");

    Res<Style::Prop> resProp = Error::invalidData("unknown property");

    Style::Prop::any([&]<typename T>(Meta::Type<T>) -> bool {
        if (sst.token.data != T::name())
            return false;

        if constexpr (not requires(Cursor<Sst> &c, T &t) { _parseProp(c, t); }) {
            logError("missing parser for property: {}", T::name());
            return false;
        } else {
            T prop;
            Cursor<Sst> c = sst.content;
            auto res = _parseProp(c, prop);
            if (not res)
                logError("failed to parse property {#}: {}", T::name(), res);
            resProp = Ok(std::move(prop));
            return true;
        }
    });

    return resProp;
}

Vec<Style::Prop> parseProperties(Sst const &sst) {
    Vec<Style::Prop> res;

    for (auto const &item : sst.content) {
        if (item == Sst::DECL) {
            auto prop = parseProperty(item);
            if (prop)
                res.pushBack(prop.take());
            else
                logWarn("failed to parse property: {}", prop.none());
        } else {
            logWarn("unexpected item in properties: {}", item.type);
        }
    }

    return res;
}

// MARK: Rules -----------------------------------------------------------------

Style::StyleRule parseStyleRule(Sst const &sst) {
    if (sst != Sst::RULE)
        panic("expected rule");

    if (sst.prefix != Sst::LIST)
        panic("expected list");

    Style::StyleRule res;

    // Parse the selector.
    auto &prefix = sst.prefix.unwrap();
    Cursor<Sst> prefixContent = prefix->content;
    res.selector = parseSelector(prefixContent);

    // Parse the properties.
    for (auto const &item : sst.content) {
        if (item == Sst::DECL) {
            auto prop = parseProperty(item);
            if (prop)
                res.props.pushBack(prop.take());
            else
                logWarn("failed to parse property: {}", prop.none());
        } else {
            logWarn("unexpected item in style rule: {}", item.type);
        }
    }

    return res;
}

Style::ImportRule parseImportRule(Sst const &) {
    return {};
}

Style::MediaRule parseMediaRule(Sst const &sst) {
    if (sst != Sst::RULE)
        panic("expected rule");

    if (sst.prefix != Sst::LIST)
        panic("expected list");

    Style::MediaRule res;

    // Parse the media query.
    auto &prefix = sst.prefix.unwrap();
    Cursor<Sst> prefixContent = prefix->content;
    res.media = parseMediaQuery(prefixContent);

    // Parse the rules.
    for (auto const &item : sst.content) {
        if (item == Sst::RULE) {
            res.rules.pushBack(parseRule(item));
        } else {
            logWarn("unexpected item in media rule: {}", item.type);
        }
    }

    return res;
}

Style::FontFaceRule parseFontFaceRule(Sst const &sst) {
    Style::FontFaceRule res;

    for (auto const &item : sst.content) {
        if (item == Sst::DECL) {
            auto prop = parseProperty(item);
            if (prop)
                res.props.pushBack(prop.take());
        } else {
            logWarn("unexpected item in font-face rule: {}", item.type);
        }
    }

    return res;
}

Style::Rule parseRule(Sst const &sst) {
    if (sst != Sst::RULE)
        panic("expected rule");

    auto tok = sst.token;
    if (tok.data == "@media")
        return parseMediaRule(sst);
    else if (tok.data == "@import")
        return parseImportRule(sst);
    else if (tok.data == "@font-face")
        return parseFontFaceRule(sst);
    else
        return parseStyleRule(sst);
}

// MARK: Stylesheets -----------------------------------------------------------

Style::StyleSheet parseStyleSheet(Sst const &sst) {
    if (sst != Sst::LIST)
        panic("expected list");

    Style::StyleSheet res;
    for (auto const &item : sst.content) {
        if (item == Sst::RULE) {
            res.rules.pushBack(parseRule(item));
        } else {
            logWarn("unexpected item in stylesheet: {}", item.type);
        }
    }

    return res;
}

} // namespace Web::Css
