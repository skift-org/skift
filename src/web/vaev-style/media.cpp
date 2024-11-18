#include "media.h"

#include "base.h"
#include "values.h"

namespace Vaev::Style {

static Cons<Style::RangePrefix, Str> _explodeFeatureName(Io::SScan s) {
    if (s.skip("min-"))
        return {Style::RangePrefix::MIN, s.remStr()};
    else if (s.skip("max-"))
        return {Style::RangePrefix::MAX, s.remStr()};
    else
        return {Style::RangePrefix::EXACT, s.remStr()};
}

static Style::Feature _parseMediaFeature(Cursor<Css::Sst> &c) {
    if (c.ended()) {
        logWarn("unexpected end of input");
        return Style::TypeFeature{MediaType::OTHER};
    }

    if (*c != Css::Token::IDENT) {
        logWarn("expected ident");
        return Style::TypeFeature{MediaType::OTHER};
    }

    auto unexplodedName = c.next().token.data;
    auto [prefix, name] = _explodeFeatureName(unexplodedName.str());

    Opt<Style::Feature> prop;

    eatWhitespace(c);
    Style::Feature::any([&]<typename F>(Meta::Type<F>) -> bool {
        if (name != F::name())
            return false;

        if (not c.skip(Css::Token::COLON)) {
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

Style::MediaQuery _parseMediaQueryInfix(Cursor<Css::Sst> &c);

Style::MediaQuery _parseMediaQueryLeaf(Cursor<Css::Sst> &c) {
    if (c.skip(Css::Token::ident("not"))) {
        return Style::MediaQuery::negate(_parseMediaQueryInfix(c));
    } else if (c.skip(Css::Token::ident("only"))) {
        return _parseMediaQueryInfix(c);
    } else if (c.peek() == Css::Sst::BLOCK) {
        Cursor<Css::Sst> content = c.next().content;
        return _parseMediaQueryInfix(content);
    } else if (auto type = parseValue<MediaType>(c)) {
        return Style::TypeFeature{type.take()};
    } else
        return _parseMediaFeature(c);
}

Style::MediaQuery _parseMediaQueryInfix(Cursor<Css::Sst> &c) {
    auto lhs = _parseMediaQueryLeaf(c);
    while (not c.ended()) {
        if (c.skip(Css::Token::ident("and"))) {
            lhs = Style::MediaQuery::combineAnd(lhs, _parseMediaQueryLeaf(c));
        } else if (c.skip(Css::Token::ident("or"))) {
            lhs = Style::MediaQuery::combineOr(lhs, _parseMediaQueryLeaf(c));
        } else {
            break;
        }
    }
    return lhs;
}

Style::MediaQuery parseMediaQuery(Cursor<Css::Sst> &c) {
    eatWhitespace(c);
    Style::MediaQuery lhs = _parseMediaQueryInfix(c);
    eatWhitespace(c);
    while (not c.ended() and c.skip(Css::Token::COMMA)) {
        eatWhitespace(c);
        auto rhs = _parseMediaQueryInfix(c);
        lhs = Style::MediaQuery::combineOr(lhs, rhs);
        eatWhitespace(c);
    }

    return lhs;
}

} // namespace Vaev::Style
