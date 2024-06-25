#include "values.h"

namespace Vaev::Css {

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
    if (c.ended())
        return Error::invalidData("unexpected end of input");

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
    }

    return Error::invalidData("expected color");
}

// MARK: Display
// https://drafts.csswg.org/css-display-3/#propdef-display

static Res<Display> _parseLegacyDisplay(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("inline-block"))) {
        return Ok(Display{Display::FLOW_ROOT, Display::INLINE});
    } else if (c.skip(Token::ident("inline-table"))) {
        return Ok(Display{Display::TABLE, Display::INLINE});
    } else if (c.skip(Token::ident("inline-flex"))) {
        return Ok(Display{Display::FLEX, Display::INLINE});
    } else if (c.skip(Token::ident("inline-grid"))) {
        return Ok(Display{Display::GRID, Display::INLINE});
    }

    return Error::invalidData("expected legacy display value");
}

static Res<Display::Outside> _parseOutsideDisplay(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("block"))) {
        return Ok(Display::BLOCK);
    } else if (c.skip(Token::ident("inline"))) {
        return Ok(Display::INLINE);
    } else if (c.skip(Token::ident("run-in"))) {
        return Ok(Display::RUN_IN);
    }

    return Error::invalidData("expected outside value");
}

static Res<Display::Inside> _parseInsideDisplay(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("flow"))) {
        return Ok(Display::FLOW);
    } else if (c.skip(Token::ident("flow-root"))) {
        return Ok(Display::FLOW_ROOT);
    } else if (c.skip(Token::ident("flex"))) {
        return Ok(Display::FLEX);
    } else if (c.skip(Token::ident("grid"))) {
        return Ok(Display::GRID);
    } else if (c.skip(Token::ident("ruby"))) {
        return Ok(Display::RUBY);
    } else if (c.skip(Token::ident("table"))) {
        return Ok(Display::TABLE);
    } else if (c.skip(Token::ident("math"))) {
        return Ok(Display::MATH);
    }

    return Error::invalidData("expected inside value");
}

static Res<Display> _parseInternalDisplay(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("table-row-group"))) {
        return Ok(Display::TABLE_ROW_GROUP);
    } else if (c.skip(Token::ident("table-header-group"))) {
        return Ok(Display::TABLE_HEADER_GROUP);
    } else if (c.skip(Token::ident("table-footer-group"))) {
        return Ok(Display::TABLE_FOOTER_GROUP);
    } else if (c.skip(Token::ident("table-row"))) {
        return Ok(Display::TABLE_ROW);
    } else if (c.skip(Token::ident("table-cell"))) {
        return Ok(Display::TABLE_CELL);
    } else if (c.skip(Token::ident("table-column-group"))) {
        return Ok(Display::TABLE_COLUMN_GROUP);
    } else if (c.skip(Token::ident("table-column"))) {
        return Ok(Display::TABLE_COLUMN);
    } else if (c.skip(Token::ident("table-caption"))) {
        return Ok(Display::TABLE_CAPTION);
    } else if (c.skip(Token::ident("ruby-base"))) {
        return Ok(Display::RUBY_BASE);
    } else if (c.skip(Token::ident("ruby-text"))) {
        return Ok(Display::RUBY_TEXT);
    } else if (c.skip(Token::ident("ruby-base-container"))) {
        return Ok(Display::RUBY_BASE_CONTAINER);
    } else if (c.skip(Token::ident("ruby-text-container"))) {
        return Ok(Display::RUBY_TEXT_CONTAINER);
    }

    return Error::invalidData("expected internal value");
}

static Res<Display> _parseBoxDisplay(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("contents"))) {
        return Ok(Display::CONTENTS);
    } else if (c.skip(Token::ident("none"))) {
        return Ok(Display::NONE);
    }

    return Error::invalidData("expected box value");
}

Res<Display> parseDisplay(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (auto legacy = _parseLegacyDisplay(c))
        return legacy;

    if (auto box = _parseBoxDisplay(c))
        return box;

    if (auto internal = _parseInternalDisplay(c))
        return internal;

    auto inside = _parseInsideDisplay(c);
    auto outside = _parseOutsideDisplay(c);
    auto item = c.skip(Token::ident("list-item"))
                    ? Display::Item::YES
                    : Display::Item::NO;

    if (not inside and not outside and not bool(item))
        return Error::invalidData("expected display value");

    return Ok(Display{
        tryOr(inside, Display::FLOW),
        tryOr(outside, Display::BLOCK),
        item,
    });
}

// MARK: Length
// https://drafts.csswg.org/css-values/#lengths

static Res<Length::Unit> _parseLengthUnit(Str unit) {
#define LENGTH(NAME, ...)      \
    if (eqCi(unit, #NAME ""s)) \
        return Ok(Length::Unit::NAME);
#include <vaev-base/defs/lengths.inc>
#undef LENGTH

    return Error::invalidData("unknown length unit");
}

Res<Length> parseLength(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::DIMENSION) {
        Io::SScan scan = c->token.data;
        auto value = tryOr(Io::atof(scan), 0.0);
        auto unit = try$(_parseLengthUnit(scan.remStr()));
        return Ok(Length{value, unit});
    }

    return Error::invalidData("expected length");
}

// MARL: MarginWidth
// https://drafts.csswg.org/css-values/#margin-width

Res<MarginWidth> parseMarginWidth(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c->token == Token::ident("auto")) {
        c.next();
        return Ok(MarginWidth::AUTO);
    }

    return Ok(try$(parseLengthOrPercentage(c)));
}

// MARK: Percentage
// https://drafts.csswg.org/css-values/#percentages

Res<Percent> parsePercentage(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::PERCENTAGE) {
        Io::SScan scan = c->token.data;
        auto value = tryOr(Io::atof(scan), 0.0);
        if (scan.remStr() != "%")
            return Error::invalidData("invalid percentage");

        return Ok(Percent{value});
    }

    return Error::invalidData("expected percentage");
}

Res<PercentOr<Length>> parseLengthOrPercentage(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::DIMENSION)
        return Ok(try$(parseLength(c)));
    else if (c.peek() == Token::PERCENTAGE)
        return Ok(try$(parsePercentage(c)));
    else
        return Error::invalidData("expected length or percentage");
}

// MARK: Size
// https://drafts.csswg.org/css-sizing-4/#sizing-values

Res<Size> parseSize(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::IDENT) {
        auto data = c.next().token.data;
        if (data == "auto") {
            return Ok(Size::AUTO);
        } else if (data == "none") {
            return Ok(Size::NONE);
        } else if (data == "min-content") {
            return Ok(Size::MIN_CONTENT);
        } else if (data == "max-content") {
            return Ok(Size::MAX_CONTENT);
        }
    } else if (c.peek() == Token::PERCENTAGE) {
        return Ok(try$(parsePercentage(c)));
    } else if (c.peek() == Token::DIMENSION) {
        return Ok(try$(parseLength(c)));
    } else if (c.peek() == Sst::FUNC) {
        auto const &prefix = c.next().prefix.unwrap();
        auto prefixToken = prefix->token;
        if (prefixToken.data == "fit-content") {
            Cursor<Sst> content = prefix->content;
            return Ok(Size{Size::FIT_CONTENT, try$(parseLength(content))});
        }
    }

    return Error::invalidData("expected size");
}

} // namespace Vaev::Css
