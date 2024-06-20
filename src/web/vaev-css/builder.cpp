#include "builder.h"

namespace Vaev::Css {

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
static Res<Display> parseDisplay(Cursor<Sst> &c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    auto const &sst = c.next();

    if (sst.token == Token{Token::IDENT, "contents"}) {
        return Ok(Display::CONTENTS);
    } else if (sst.token == Token{Token::IDENT, "none"}) {
        return Ok(Display::NONE);
    }

    return Error::invalidData("expected display value");
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

    if (c->token == Token{Token::IDENT, "auto"}) {
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

// MARK: Media Queries ---------------------------------------------------------

Style::MediaQuery parseMediaQuery(Cursor<Sst> &) {
    return {};
}

// MARK: Selectors -------------------------------------------------------------

enum struct OpCode {
    DESCENDANT,
    CHILD,
    ADJACENT,
    SUBSEQUENT,
    NOT,
    WHERE,
    AND,
    COLUMN,
    OR,
    NOP
};

static Style::Selector parseSelectorElement(Vec<Sst> const &prefix, usize &i) {

    switch (prefix[i].token.type) {
    case Token::WHITESPACE:
        if (i >= prefix.len()) {
            logError("ERROR : unterminated selector");
            return Style::EmptySelector{};
        }
        i++;
        return parseSelectorElement(prefix, i);
    case Token::HASH:
        return Style::IdSelector{next(prefix[i].token.data, 1)};
    case Token::IDENT:
        return Style::TypeSelector{TagName::make(prefix[i].token.data, Vaev::HTML)};
    case Token::DELIM:
        if (prefix[i].token.data == ".") {
            if (i >= prefix.len()) {
                logError("ERROR : unterminated selector");
                return Style::EmptySelector{};
            }
            i++;
            return Style::ClassSelector{prefix[i].token.data};
        } else if (prefix[i].token.data == "*") {
            return Style::UniversalSelector{};
        }

    default:
        return Style::ClassSelector{prefix[i].token.data};
    }
}

static OpCode sstNode2OpCode(Vec<Sst> const &content, usize &i) {
    switch (content[i].token.type) {
    case Token::Type::COMMA:
        if (i >= content.len() - 1) {
            logError("ERROR : unterminated selector");
            return OpCode::NOP;
        }
        i++;
        return OpCode::OR;
    case Token::Type::WHITESPACE:
        // a white space could be an operator or be ignored if followed by another op
        if (i >= content.len() - 1) {
            return OpCode::NOP;
        }
        if (content[i + 1].token.type == Token::Type::IDENT || content[i + 1].token.type == Token::Type::HASH || content[i + 1].token.data == "*") {
            i++;
            return OpCode::DESCENDANT;
        } else {
            i++;
            auto op = sstNode2OpCode(content, i);
            if (i >= content.len() - 1) {
                logError("ERROR : unterminated selector");
                return OpCode::NOP;
            }
            if (content[i + 1].token.type == Token::Type::WHITESPACE) {
                i++;
            }
            return op;
        }
    default:
        return OpCode::AND;
    }
}

Style::Selector parseNfixExpr(auto lhs, OpCode op, Vec<Sst> const &content, usize &i) {
    Vec<Style::Selector> selectors = {lhs, parseSelectorElement(content, i)};

    // TODO parse next selectors

    switch (op) {
    case OpCode::AND:
        return Style::Selector::and_(selectors);
    case OpCode::OR:
        return Style::Selector::or_(selectors);
    default:
        return Style::Selector::and_(selectors);
    }
}

Style::Selector parseInfixExpr(auto lhs, auto content, usize &i) {
    OpCode opCode = sstNode2OpCode(content, i);

    switch (opCode) {
    case OpCode::NOP:
        return lhs;
    case OpCode::DESCENDANT:
        return Style::Selector::descendant(lhs, parseSelectorElement(content, i));
    case OpCode::CHILD:
        return Style::Selector::child(lhs, parseSelectorElement(content, i));
    case OpCode::ADJACENT:
        return Style::Selector::adjacent(lhs, parseSelectorElement(content, i));
    case OpCode::SUBSEQUENT:
        return Style::Selector::subsequent(lhs, parseSelectorElement(content, i));
    case OpCode::NOT:
        return Style::Selector::not_(parseSelectorElement(content, i));
    case OpCode::WHERE:
        return Style::Selector::where(parseSelectorElement(content, i));
    case OpCode::COLUMN:
    case OpCode::OR:
    case OpCode::AND:
        return parseNfixExpr(lhs, opCode, content, i);
    }
}

Style::Selector parseSelector(auto &content) {
    usize i = 0;
    Style::Selector currentSelector = parseSelectorElement(content, i);
    if (i >= content.len() - 1) {
        return currentSelector;
    }
    i++;
    return parseInfixExpr(currentSelector, content, i);
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

Res<> _parseProp(Cursor<Sst> &c, Style::DisplayProp &p) {
    p.value = try$(parseDisplay(c));
    return Ok();
}

// MARK: Margin

Res<> _parseProp(Cursor<Sst> &c, Style::MarginTopProp &p) {
    p.value = try$(parseMarginWidth(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MarginRightProp &p) {
    p.value = try$(parseMarginWidth(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MarginBottomProp &p) {
    p.value = try$(parseMarginWidth(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MarginLeftProp &p) {
    p.value = try$(parseMarginWidth(c));
    return Ok();
}

// MARK: Padding

Res<> _parseProp(Cursor<Sst> &c, Style::PaddingTopProp &p) {
    p.value = try$(parseLengthOrPercentage(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::PaddingRightProp &p) {
    p.value = try$(parseLengthOrPercentage(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::PaddingBottomProp &p) {
    p.value = try$(parseLengthOrPercentage(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::PaddingLeftProp &p) {
    p.value = try$(parseLengthOrPercentage(c));
    return Ok();
}

// MARK: Sizing

Res<> _parseProp(Cursor<Sst> &c, Style::WidthProp &p) {
    p.value = try$(parseSize(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::HeightProp &p) {
    p.value = try$(parseSize(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MinWidthProp &p) {
    p.value = try$(parseSize(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MinHeightProp &p) {
    p.value = try$(parseSize(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MaxWidthProp &p) {
    p.value = try$(parseSize(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MaxHeightProp &p) {
    p.value = try$(parseSize(c));
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
    res.selector = parseSelector(prefix->content);

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

} // namespace Vaev::Css
