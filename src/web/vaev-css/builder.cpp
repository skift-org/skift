#include "builder.h"

#include "selectors.h"
#include "values.h"

namespace Vaev::Css {

// MARK: Media Queries ---------------------------------------------------------

Style::MediaQuery parseMediaQuery(Cursor<Sst> &) {
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