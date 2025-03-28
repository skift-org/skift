#include "media.h"

#include "base.h"
#include "values.h"

namespace Vaev::Style {

static Pair<RangePrefix, Str> _explodeFeatureName(Io::SScan s) {
    if (s.skip("min-"))
        return {RangePrefix::MIN, s.remStr()};
    else if (s.skip("max-"))
        return {RangePrefix::MAX, s.remStr()};
    else
        return {RangePrefix::EXACT, s.remStr()};
}

static Feature _parseMediaFeature(Cursor<Css::Sst>& c) {
    if (c.ended()) {
        logWarn("unexpected end of input");
        return TypeFeature{MediaType::OTHER};
    }

    if (*c != Css::Token::IDENT) {
        logWarn("expected ident");
        return TypeFeature{MediaType::OTHER};
    }

    auto unexplodedName = c.next().token.data;
    auto [prefix, name] = _explodeFeatureName(unexplodedName.str());

    Opt<Feature> prop;

    eatWhitespace(c);
    Feature::any([&]<typename F>() -> bool {
        if (name != F::name())
            return false;

        if (not c.skip(Css::Token::COLON)) {
            prop.emplace(F::make(RangePrefix::BOOL));
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
        return TypeFeature{MediaType::OTHER};
    }

    return prop.take();
}

MediaQuery _parseMediaQueryInfix(Cursor<Css::Sst>& c);

MediaQuery _parseMediaQueryLeaf(Cursor<Css::Sst>& c) {
    if (c.skip(Css::Token::ident("not"))) {
        return MediaQuery::negate(_parseMediaQueryInfix(c));
    } else if (c.skip(Css::Token::ident("only"))) {
        return _parseMediaQueryInfix(c);
    } else if (c.peek() == Css::Sst::BLOCK) {
        Cursor<Css::Sst> content = c.next().content;
        return _parseMediaQueryInfix(content);
    } else if (auto type = parseValue<MediaType>(c)) {
        return TypeFeature{type.take()};
    } else
        return _parseMediaFeature(c);
}

MediaQuery _parseMediaQueryInfix(Cursor<Css::Sst>& c) {
    auto lhs = _parseMediaQueryLeaf(c);
    while (not c.ended()) {
        if (c.skip(Css::Token::ident("and"))) {
            lhs = MediaQuery::combineAnd(lhs, _parseMediaQueryLeaf(c));
        } else if (c.skip(Css::Token::ident("or"))) {
            lhs = MediaQuery::combineOr(lhs, _parseMediaQueryLeaf(c));
        } else {
            break;
        }
    }
    return lhs;
}

MediaQuery parseMediaQuery(Cursor<Css::Sst>& c) {
    eatWhitespace(c);
    MediaQuery lhs = _parseMediaQueryInfix(c);
    eatWhitespace(c);
    while (not c.ended() and c.skip(Css::Token::COMMA)) {
        eatWhitespace(c);
        auto rhs = _parseMediaQueryInfix(c);
        lhs = MediaQuery::combineOr(lhs, rhs);
        eatWhitespace(c);
    }

    return lhs;
}

} // namespace Vaev::Style
