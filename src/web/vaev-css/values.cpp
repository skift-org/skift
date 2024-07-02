#include "values.h"

namespace Vaev::Css {

// NOTE: Please keep this alphabetically sorted.

// MARK: Boolean
// https://drafts.csswg.org/mediaqueries/#grid
Res<bool> ValueParser<bool>::parse(Cursor<Sst> &c) {
    auto val = try$(parseValue<Integer>(c));
    return Ok(val > 0);
}

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

Res<Color> ValueParser<Color>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::HASH) {
        Io::SScan scan = c->token.data;
        c.next();
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

// MARK: Color Gamut
// https://drafts.csswg.org/mediaqueries/#color-gamut
Res<ColorGamut> ValueParser<ColorGamut>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("srgb")))
        return Ok(ColorGamut::SRGB);
    else if (c.skip(Token::ident("p3")))
        return Ok(ColorGamut::DISPLAY_P3);
    else if (c.skip(Token::ident("rec2020")))
        return Ok(ColorGamut::REC2020);
    else
        return Error::invalidData("expected color gamut");
}

// MARK: Display
// https://drafts.csswg.org/css-display-3/#propdef-display

static Res<Display>
_parseLegacyDisplay(Cursor<Sst> &c) {
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

Res<Display> ValueParser<Display>::parse(Cursor<Sst> &c) {
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

// MARK: Hover
// https://drafts.csswg.org/mediaqueries/#hover
Res<Hover> ValueParser<Hover>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("none")))
        return Ok(Hover::NONE);
    else if (c.skip(Token::ident("hover")))
        return Ok(Hover::HOVER);
    else
        return Error::invalidData("expected hover value");
}

// MARK: Integer
// https://drafts.csswg.org/css-values/#integers
Res<Integer> ValueParser<Integer>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::NUMBER) {
        Io::SScan scan = c->token.data;
        return Ok(try$(Io::atoi(scan)));
    }

    return Error::invalidData("expected integer");
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

Res<Length> ValueParser<Length>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::DIMENSION) {
        Io::SScan scan = c->token.data;
        auto value = tryOr(Io::atof(scan), 0.0);
        auto unit = try$(_parseLengthUnit(scan.remStr()));
        return Ok(Length{value, unit});
    } else if (c.peek() == Token::number("0")) {
        return Ok(Length{0.0, Length::Unit::PX});
    }

    return Error::invalidData("expected length");
}

// MARL: MarginWidth
// https://drafts.csswg.org/css-values/#margin-width

Res<MarginWidth> ValueParser<MarginWidth>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c->token == Token::ident("auto")) {
        c.next();
        return Ok(MarginWidth::AUTO);
    }

    return Ok(try$(parseValue<PercentOr<Length>>(c)));
}

// MARK: MediaType
// https://drafts.csswg.org/mediaqueries/#media-types

Res<MediaType> ValueParser<MediaType>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("all")))
        return Ok(MediaType::ALL);

    if (c.skip(Token::ident("print")))
        return Ok(MediaType::PRINT);

    if (c.skip(Token::ident("screen")))
        return Ok(MediaType::SCREEN);

    // NOTE: The spec says that we should reconize the following media types
    //       but they should not match anything
    static constexpr Array OTHER = {
        "tty",
        "tv",
        "projection",
        "handheld",
        "braille",
        "embossed",
        "speech",
        "aural",
    };

    for (auto const &item : OTHER) {
        if (c.skip(Token::ident(item)))
            return Ok(MediaType::OTHER);
    }
    return Error::invalidData("expected media type");
}

// MARK: Number
// https://drafts.csswg.org/css-values/#numbers

Res<Number> ValueParser<Number>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::NUMBER) {
        Io::SScan scan = c->token.data;
        return Ok(try$(Io::atof(scan)));
    }

    return Error::invalidData("expected number");
}

// MARK: Orientation
// https://drafts.csswg.org/mediaqueries/#orientation
Res<Orientation> ValueParser<Orientation>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("portrait")))
        return Ok(Orientation::PORTRAIT);
    else if (c.skip(Token::ident("landscape")))
        return Ok(Orientation::LANDSCAPE);
    else
        return Error::invalidData("expected orientation");
}

// MARK: OverflowBlock
// https://drafts.csswg.org/mediaqueries/#mf-overflow-block
Res<OverflowBlock> ValueParser<OverflowBlock>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("none")))
        return Ok(OverflowBlock::NONE);
    else if (c.skip(Token::ident("scroll")))
        return Ok(OverflowBlock::SCROLL);
    else if (c.skip(Token::ident("paged")))
        return Ok(OverflowBlock::PAGED);
    else
        return Error::invalidData("expected overflow block value");
}

// MARK: OverflowInline
// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline
Res<OverflowInline> ValueParser<OverflowInline>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("none")))
        return Ok(OverflowInline::NONE);
    else if (c.skip(Token::ident("scroll")))
        return Ok(OverflowInline::SCROLL);
    else
        return Error::invalidData("expected overflow inline value");
}

// MARK: Percentage
// https://drafts.csswg.org/css-values/#percentages

Res<Percent> ValueParser<Percent>::parse(Cursor<Sst> &c) {
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

// MARK: Pointer
// https://drafts.csswg.org/mediaqueries/#pointer
Res<Pointer> ValueParser<Pointer>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("none")))
        return Ok(Pointer::NONE);
    else if (c.skip(Token::ident("coarse")))
        return Ok(Pointer::COARSE);
    else if (c.skip(Token::ident("fine")))
        return Ok(Pointer::FINE);
    else
        return Error::invalidData("expected pointer value");
}

// Mark: Resolution
// https://drafts.csswg.org/css-values/#resolution

static Res<Resolution::Unit> _parseResolutionUnit(Str unit) {
    if (eqCi(unit, "dpi"s))
        return Ok(Resolution::Unit::DPI);
    else if (eqCi(unit, "dpcm"s))
        return Ok(Resolution::Unit::DPCM);
    else if (eqCi(unit, "dppx"s))
        return Ok(Resolution::Unit::DPPX);
    else
        return Error::invalidData("unknown resolution unit");
}

Res<Resolution> ValueParser<Resolution>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::DIMENSION) {
        Io::SScan scan = c->token.data;
        auto value = tryOr(Io::atof(scan), 0.0);
        auto unit = try$(_parseResolutionUnit(scan.remStr()));
        return Ok(Resolution{value, unit});
    }

    return Error::invalidData("expected resolution");
}

// MARK: Scan
// https://drafts.csswg.org/mediaqueries/#scan
Res<Scan> ValueParser<Scan>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("interlace")))
        return Ok(Scan::INTERLACE);
    else if (c.skip(Token::ident("progressive")))
        return Ok(Scan::PROGRESSIVE);
    else
        return Error::invalidData("expected scan value");
}

// MARK: Size
// https://drafts.csswg.org/css-sizing-4/#sizing-values

Res<Size> ValueParser<Size>::parse(Cursor<Sst> &c) {
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
        return Ok(try$(parseValue<Percent>(c)));
    } else if (c.peek() == Token::DIMENSION) {
        return Ok(try$(parseValue<Length>(c)));
    } else if (c.peek() == Sst::FUNC) {
        auto const &prefix = c.next().prefix.unwrap();
        auto prefixToken = prefix->token;
        if (prefixToken.data == "fit-content") {
            Cursor<Sst> content = prefix->content;
            return Ok(Size{Size::FIT_CONTENT, try$(parseValue<Length>(content))});
        }
    }

    return Error::invalidData("expected size");
}

// MARK: Update
// https://drafts.csswg.org/mediaqueries/#update
Res<Update> ValueParser<Update>::parse(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Token::ident("none")))
        return Ok(Update::NONE);
    else if (c.skip(Token::ident("slow")))
        return Ok(Update::SLOW);
    else if (c.skip(Token::ident("fast")))
        return Ok(Update::FAST);
    else
        return Error::invalidData("expected update value");
}

} // namespace Vaev::Css
