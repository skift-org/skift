#pragma once

#include <vaev-css/parser.h>
#include <vaev-style/styles.h>

#include "base.h"

namespace Vaev::Style {

template <typename T>
Res<T> parseDeclarationValue(Cursor<Css::Sst> &c) {
    if constexpr (requires { T{}.parse(c); }) {
        T t;
        try$(t.parse(c));

        return Ok(std::move(t));
    } else {
        logError("missing parser for declaration: {}", T::name());
        return Error::notImplemented("missing parser for declaration");
    }
}

template <typename P>
Res<P> parseDeclaration(Css::Sst const &sst, bool allowDeferred = true) {
    if (sst != Css::Sst::DECL)
        panic("expected declaration");

    if (sst.token != Css::Token::IDENT)
        panic("expected ident");

    Res<P> resDecl = Error::invalidData("unknown declaration");

    P::any(
        Visitor{
            [&](Meta::Type<CustomProp>) -> bool {
                if constexpr (requires(P &p) { p.template unwrap<CustomProp>(); }) {
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
                Cursor<Css::Sst> cursor = sst.content;

                auto res = parseDeclarationValue<T>(cursor);

                if (not res and allowDeferred) {
                    if constexpr (Meta::Constructible<P, DeferredProp>) {
                        resDecl = Ok(DeferredProp{T::name(), sst.content});
                        return true;
                    }
                } else if (not res) {
                    resDecl = res.none();
                    return true;
                } else {
                    resDecl = Ok(res.take());
                }

                if constexpr (requires { P::important; }) {
                    if (cursor.skip(Css::Token::delim("!")) and cursor.skip(Css::Token::ident("important")))
                        resDecl.unwrap().important = Important::YES;
                }

                eatWhitespace(cursor);
                if (not cursor.ended())
                    resDecl = Error::invalidData("unknown tokens in content");

                return true;
            }
        } // namespace Vaev::Style
    );

    if (not resDecl)
        logWarn("failed to parse declaration: {} - {}", sst, resDecl);

    return resDecl;
}

template <typename P>
Vec<P> parseDeclarations(Css::Content const &sst, bool allowDeferred = true) {
    Vec<P> res;

    for (auto const &item : sst) {
        if (item != Css::Sst::DECL) {
            logWarn("unexpected item in declaration: {}", item.type);
            continue;
        }

        auto prop = parseDeclaration<P>(item, allowDeferred);

        if (not prop) {
            logWarn("failed to parse declaration: {}", prop.none());
            continue;
        }
        res.pushBack(prop.take());
    }

    return res;
}

template <typename P>
Vec<P> parseDeclarations(Css::Sst const &sst, bool allowDeferred = true) {
    return parseDeclarations<P>(sst.content, allowDeferred);
}

template <typename P>
Vec<P> parseDeclarations(Str style, bool allowDeferred = true) {
    Css::Lexer lex{style};
    auto sst = Css::consumeDeclarationList(lex, true);
    return parseDeclarations<P>(sst, allowDeferred);
}

} // namespace Vaev::Style
