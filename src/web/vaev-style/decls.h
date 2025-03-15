#pragma once

#include "base.h"
#include "css/parser.h"
#include "props.h"

namespace Vaev::Style {

static bool DEBUG_DECL = false;

template <typename T>
Res<T> parseDeclarationValue(Cursor<Css::Sst>& c) {
    if constexpr (requires { T{}.parse(c); }) {
        T t;
        try$(t.parse(c));

        return Ok(std::move(t));
    } else {
        logErrorIf(DEBUG_DECL, "missing parser for declaration: {}", T::name());
        return Error::notImplemented("missing parser for declaration");
    }
}

static inline Important _consumeImportant(Cursor<Css::Sst>& c, bool eatEverything) {
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
static inline P _deferProperty(Css::Sst const& sst) {
    P prop = DeferredProp{sst.token.data, sst.content};
    if constexpr (requires { P::important; }) {
        Cursor<Css::Sst> content = sst.content;
        prop.important = _consumeImportant(content, true);
    }
    return prop;
}

template <typename P, typename T>
static inline Res<P> _parseDeclaration(Css::Sst const& sst) {
    Cursor<Css::Sst> content = sst.content;

    eatWhitespace(content);
    P prop = try$(parseDeclarationValue<T>(content));

    if constexpr (requires { P::important; })
        prop.important = _consumeImportant(content, false);

    if (not content.ended())
        return Error::invalidData("unknown tokens in content");

    return Ok(std::move(prop));
}

template <typename P>
static inline Res<P> _parseDefaulted(Css::Sst const& sst) {
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

template <typename P>
Res<P> parseDeclaration(Css::Sst const& sst, bool allowDeferred = true) {
    if (sst != Css::Sst::DECL)
        panic("expected declaration");

    if (sst.token != Css::Token::IDENT)
        panic("expected ident");

    Res<P> resDecl = Error::invalidData("unknown declaration");

    P::any(
        Visitor{
            [&](Meta::Type<CustomProp>) -> bool {
                if constexpr (requires(P& p) { p.template unwrap<CustomProp>(); }) {
                    if (startWith(sst.token.data, "--"s) == Match::NO) {
                        return false;
                    }

                    resDecl = Ok(CustomProp(sst.token.data, sst.content));
                    return true;
                }
                return false;
            },
            [&]<typename T>(Meta::Type<T>) -> bool {
                if (sst.token != Css::Token::ident(T::name())) {
                    return false;
                }

                resDecl = _parseDeclaration<P, T>(sst);

                if constexpr (Meta::Constructible<P, DefaultedProp>) {
                    if (not resDecl) {
                        resDecl = _parseDefaulted<P>(sst);
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

template <typename P>
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

template <typename P>
Vec<P> parseDeclarations(Css::Sst const& sst, bool allowDeferred = true) {
    return parseDeclarations<P>(sst.content, allowDeferred);
}

template <typename P>
Vec<P> parseDeclarations(Str style, bool allowDeferred = true) {
    Css::Lexer lex{style};
    auto sst = Css::consumeDeclarationList(lex, true);
    return parseDeclarations<P>(sst, allowDeferred);
}

} // namespace Vaev::Style
