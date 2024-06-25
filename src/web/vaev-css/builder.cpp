#include "builder.h"

#include "values.h"

namespace Vaev::Css {

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
