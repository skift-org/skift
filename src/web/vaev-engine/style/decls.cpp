module;

#include <karm-logger/logger.h>
#include <karm-math/au.h>

export module Vaev.Engine:style.decls;

import :css;
import :style.props;

namespace Vaev::Style {

[[gnu::used]]
static bool DEBUG_DECL = false;

template <typename T>
Res<T> _parseDeclarationValue(Cursor<Css::Sst>& c) {
    if constexpr (requires { T{}.parse(c); }) {
        T t;
        try$(t.parse(c));

        return Ok(std::move(t));
    } else {
        logErrorIf(DEBUG_DECL, "missing parser for declaration: {}", T::name());
        return Error::notImplemented("missing parser for declaration");
    }
}

Important _consumeImportant(Cursor<Css::Sst>& c, bool eatEverything) {
    eatWhitespace(c);
    while (not c.ended()) {
        if (c.skip(Css::Token::delim("!")) and
            c.skip(Css::Token::ident("important"))) {

            eatWhitespace(c);
            return Important::YES;
        }
        if (not eatEverything)
            break;
        c.next();
    }

    return Important::NO;
}

template <typename P>
P _deferProperty(Css::Sst const& sst) {
    P prop = DeferredProp{
        Symbol::from(sst.token.data),
        sst.content,
    };
    if constexpr (requires { P::important; }) {
        Cursor<Css::Sst> content = sst.content;
        prop.important = _consumeImportant(content, true);
    }
    return prop;
}

template <typename P, typename T>
Res<P> _parseDeclaration(Css::Sst const& sst) {
    Cursor<Css::Sst> content = sst.content;

    eatWhitespace(content);
    P prop = try$(_parseDeclarationValue<T>(content));

    if constexpr (requires { P::important; })
        prop.important = _consumeImportant(content, false);

    if (not content.ended()) {
        logDebugIf(DEBUG_DECL, "unknown tokens in content: {}", content);
        return Error::invalidData("unknown tokens in content");
    }

    return Ok(std::move(prop));
}

template <typename P>
Res<P> _parseDefaulted(Css::Sst const& sst) {
    Cursor<Css::Sst> content = sst.content;
    Res<P> res = Error::invalidData("unknown declaration");
    if (content.skip(Css::Token::ident("initial"))) {
        res = Ok(DefaultedProp{sst.token.data, Default::INITIAL});
    } else if (content.skip(Css::Token::ident("inherit"))) {
        res = Ok(DefaultedProp{sst.token.data, Default::INHERIT});
    } else if (content.skip(Css::Token::ident("unset"))) {
        res = Ok(DefaultedProp{sst.token.data, Default::UNSET});
    } else if (content.skip(Css::Token::ident("revert"))) {
        res = Ok(DefaultedProp{sst.token.data, Default::REVERT});
    }
    return res;
}

export template <typename P>
Res<P> parseDeclaration(Css::Sst const& sst, bool allowDeferred = true) {
    if (sst != Css::Sst::DECL)
        panic("expected declaration");

    if (sst.token != Css::Token::IDENT)
        panic("expected ident");

    Res<P> resDecl = Error::invalidData("unknown declaration");

    P::any(
        Visitor{
            [&]<Meta::Same<CustomProp>>() -> bool {
                if constexpr (requires(P& p) { p.template unwrap<CustomProp>(); }) {
                    if (startWith(sst.token.data, "--"s) == Match::NO) {
                        return false;
                    }

                    resDecl = Ok(CustomProp{
                        Symbol::from(sst.token.data),
                        sst.content,
                    });
                    return true;
                }
                return false;
            },
            [&]<typename T>() -> bool {
                if (sst.token != Css::Token::ident(T::name()))
                    return false;

                resDecl = _parseDeclaration<P, T>(sst);

                if constexpr (Meta::Constructible<P, DefaultedProp>) {
                    if (not resDecl) {
                        auto resDefault = _parseDefaulted<P>(sst);
                        if (resDefault)
                            resDecl = std::move(resDefault);
                    }
                }

                if constexpr (Meta::Constructible<P, DeferredProp>) {
                    if (not resDecl and allowDeferred) {
                        resDecl = Ok(_deferProperty<P>(sst));
                    }
                }

                return true;
            }
        }
    );

    if (not resDecl)
        logWarnIf(DEBUG_DECL, "failed to parse declaration: {} - {}", sst, resDecl);

    return resDecl;
}

export template <typename P>
Vec<P> parseDeclarations(Css::Content const& sst, bool allowDeferred = true) {
    Vec<P> res;

    for (auto const& item : sst) {
        if (item != Css::Sst::DECL) {
            logWarnIf(DEBUG_DECL, "unexpected item in declaration: {}", item.type);
            continue;
        }

        auto prop = parseDeclaration<P>(item, allowDeferred);

        if (not prop) {
            logWarnIf(DEBUG_DECL, "failed to parse declaration: {}", prop.none());
            continue;
        }
        res.pushBack(prop.take());
    }

    return res;
}

export template <typename P>
Vec<P> parseDeclarations(Css::Sst const& sst, bool allowDeferred = true) {
    return parseDeclarations<P>(sst.content, allowDeferred);
}

export template <typename P>
Vec<P> parseDeclarations(Str style, bool allowDeferred = true) {
    Css::Lexer lex{style};
    auto sst = Css::consumeDeclarationList(lex, true);
    return parseDeclarations<P>(sst, allowDeferred);
}

// MARK: SVG Presentation Attributes -------------------------------------------

String wrapPathAsCSSStyle(Str style) {
    StringBuilder sb;
    sb.append("path(\""s);
    for (auto r : iterRunes(style)) {
        if (r == '\n')
            continue;
        sb.append(r);
    }
    sb.append("\")"s);
    return sb.take();
}

void fixTransformNumberToDimensions(Vec<Css::Sst>& sst) {
    auto appendSuffix = [](String const& a, Str const& b) {
        StringBuilder sb;
        sb.append(a);
        sb.append(b);
        return sb.take();
    };

    for (auto& c : sst) {
        if (c.prefix == Css::Token::function("translate(")) {
            for (auto& tc : c.content) {
                if (tc.token.type != Css::Token::NUMBER)
                    continue;

                tc.token = {Css::Token::DIMENSION, appendSuffix(tc.token.data, "px"s)};
            }
        } else if (
            c.prefix == Css::Token::function("rotate(") or
            c.prefix == Css::Token::function("skewX(") or
            c.prefix == Css::Token::function("skewY(")
        ) {
            for (auto& tc : c.content) {
                if (tc.token.type != Css::Token::NUMBER)
                    continue;

                tc.token = {Css::Token::DIMENSION, appendSuffix(tc.token.data, "deg"s)};
            }
        }
    }
}

// https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
export void parseSVGPresentationAttribute(Symbol presentationAttr, Str style, Vec<StyleProp>& styleProps) {
    SVGStyleProp::any(
        Visitor{
            [&]<Meta::Same<TransformProp>>() -> bool {
                if (presentationAttr.str() != TransformProp::name())
                    return false;

                TransformProp t;

                Css::Lexer lex{style};
                auto sst = Css::consumeDeclarationValue(lex);
                fixTransformNumberToDimensions(sst);
                Cursor<Css::Sst> content = sst;

                if (t.parse(content))
                    styleProps.pushBack(std::move(t));
                return true;
            },
            [&]<Meta::Same<SVGDProp>>() -> bool {
                if (presentationAttr != SVGDProp::name())
                    return false;
                SVGDProp d;

                auto fixedStyle = wrapPathAsCSSStyle(style);
                Css::Lexer lex{fixedStyle};
                auto sst = Css::consumeDeclarationValue(lex);
                Cursor<Css::Sst> content = sst;

                if (d.parse(content))
                    styleProps.pushBack(std::move(d));
                return true;
            },
            [&]<typename T>() -> bool {
                if (presentationAttr != T::name())
                    return false;

                Css::Lexer lex{style};
                auto sst = Css::consumeDeclarationValue(lex);

                Cursor<Css::Sst> content = sst;
                if (auto prop = _parseDeclarationValue<T>(content)) {
                    styleProps.pushBack(std::move(prop.take()));
                } else if (auto maybeNumber = parseValue<Number>(content)) {
                    T propAsNumber;

                    if constexpr (Meta::Constructible<decltype(propAsNumber.value), Length>) {
                        propAsNumber.value = Length{Au{maybeNumber.take()}};
                    } else if constexpr (Meta::Constructible<decltype(propAsNumber.value), CalcValue<PercentOr<Length>>>) {
                        propAsNumber.value = CalcValue<PercentOr<Length>>{Length{Au{maybeNumber.take()}}};
                    } else {
                        return true;
                    }

                    styleProps.pushBack(std::move(propAsNumber));
                }

                return true;
            }
        }
    );
}

} // namespace Vaev::Style
