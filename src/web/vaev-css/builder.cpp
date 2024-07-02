#include "builder.h"

#include "selectors.h"
#include "values.h"

namespace Vaev::Css {

static void eatWhitespace(Cursor<Sst> &c) {
    while (not c.ended() and c.peek() == Token::WHITESPACE)
        c.next();
}

// MARK: Media Queries ---------------------------------------------------------

static Cons<Style::RangePrefix, Str> _explodeFeatureName(Io::SScan s) {
    if (s.skip("min-"))
        return {Style::RangePrefix::MIN, s.remStr()};
    else if (s.skip("max-"))
        return {Style::RangePrefix::MAX, s.remStr()};
    else
        return {Style::RangePrefix::EXACT, s.remStr()};
}

static Style::Feature _parseMediaFeature(Cursor<Sst> &c) {
    if (c.ended()) {
        logWarn("unexpected end of input");
        return Style::TypeFeature{MediaType::OTHER};
    }

    if (*c != Token::IDENT) {
        logWarn("expected ident");
        return Style::TypeFeature{MediaType::OTHER};
    }

    auto unexplodedName = c.next().token.data;
    auto [prefix, name] = _explodeFeatureName(unexplodedName);

    Opt<Style::Feature> prop;

    eatWhitespace(c);
    Style::Feature::any([&]<typename F>(Meta::Type<F>) -> bool {
        if (name != F::name())
            return false;

        if (not c.skip(Token::COLON)) {
            prop.emplace(F::make(Style::RangePrefix::BOOL));
            return true;
        }

        eatWhitespace(c);

        auto maybeValue = parseValue<typename F::Inner>(c);
        if (not maybeValue) {
            logError("failed to parse value for feature {#}: {}", F::name(), maybeValue.none());
            return true;
        }

        prop.emplace(F::make(prefix, maybeValue.take()));
        return true;
    });

    if (not prop) {
        logWarn("cannot parse feature: {}", unexplodedName);
        return Style::TypeFeature{MediaType::OTHER};
    }

    return prop.take();
}

Style::MediaQuery parseMediaQueryInfix(Cursor<Sst> &c);

Style::MediaQuery _parseMediaQueryLeaf(Cursor<Sst> &c) {
    if (c.skip(Token::ident("not"))) {
        return Style::MediaQuery::negate(parseMediaQueryInfix(c));
    } else if (c.skip(Token::ident("only"))) {
        return parseMediaQueryInfix(c);
    } else if (c.peek() == Sst::BLOCK) {
        Cursor<Sst> content = c.next().content;
        return parseMediaQueryInfix(content);
    } else if (auto type = parseValue<MediaType>(c)) {
        return Style::TypeFeature{type.take()};
    } else
        return _parseMediaFeature(c);
}

Style::MediaQuery parseMediaQueryInfix(Cursor<Sst> &c) {
    auto lhs = _parseMediaQueryLeaf(c);
    while (not c.ended()) {
        if (c.skip(Token::ident("and"))) {
            lhs = Style::MediaQuery::combineAnd(lhs, _parseMediaQueryLeaf(c));
        } else if (c.skip(Token::ident("or"))) {
            lhs = Style::MediaQuery::combineOr(lhs, _parseMediaQueryLeaf(c));
        } else {
            break;
        }
    }
    return lhs;
}

Style::MediaQuery parseMediaQuery(Cursor<Sst> &c) {
    eatWhitespace(c);
    Style::MediaQuery lhs = parseMediaQueryInfix(c);
    eatWhitespace(c);
    while (not c.ended() and c.skip(Token::COMMA)) {
        eatWhitespace(c);
        auto rhs = parseMediaQueryInfix(c);
        lhs = Style::MediaQuery::combineOr(lhs, rhs);
        eatWhitespace(c);
    }

    return lhs;
}

// MARK: Properties ------------------------------------------------------------

// NOTE: Please keep this alphabetically sorted.

Res<> _parseProp(Cursor<Sst> &c, Style::BackgroundColorProp &p) {
    eatWhitespace(c);
    while (not c.ended()) {
        p.value.pushBack(try$(parseValue<Color>(c)));
        eatWhitespace(c);
    }
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::ColorProp &p) {
    p.value = try$(parseValue<Color>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::DisplayProp &p) {
    p.value = try$(parseValue<Display>(c));
    return Ok();
}

// MARK: Margin

Res<> _parseProp(Cursor<Sst> &c, Style::MarginTopProp &p) {
    p.value = try$(parseValue<MarginWidth>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MarginRightProp &p) {
    p.value = try$(parseValue<MarginWidth>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MarginBottomProp &p) {
    p.value = try$(parseValue<MarginWidth>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MarginLeftProp &p) {
    p.value = try$(parseValue<MarginWidth>(c));
    return Ok();
}

// MARK: Padding

Res<> _parseProp(Cursor<Sst> &c, Style::PaddingTopProp &p) {
    p.value = try$(parseValue<PercentOr<Length>>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::PaddingRightProp &p) {
    p.value = try$(parseValue<PercentOr<Length>>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::PaddingBottomProp &p) {
    p.value = try$(parseValue<PercentOr<Length>>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::PaddingLeftProp &p) {
    p.value = try$(parseValue<PercentOr<Length>>(c));
    return Ok();
}

// MARK: Order
Res<> _parseProp(Cursor<Sst> &c, Style::OrderProp &p) {
    p.value = try$(parseValue<Integer>(c));
    return Ok();
}

// MARK: Sizing

Res<> _parseProp(Cursor<Sst> &c, Style::WidthProp &p) {
    p.value = try$(parseValue<Size>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::HeightProp &p) {
    p.value = try$(parseValue<Size>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MinWidthProp &p) {
    p.value = try$(parseValue<Size>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MinHeightProp &p) {
    p.value = try$(parseValue<Size>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MaxWidthProp &p) {
    p.value = try$(parseValue<Size>(c));
    return Ok();
}

Res<> _parseProp(Cursor<Sst> &c, Style::MaxHeightProp &p) {
    p.value = try$(parseValue<Size>(c));
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
                logWarn("failed to parse property {#}: {#} - {}", T::name(), sst.content, res);
            resProp = Ok(std::move(prop));
            return true;
        }
    });

    if (not resProp)
        logWarn("failed to parse property: {} - {}", sst.token, resProp);

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
