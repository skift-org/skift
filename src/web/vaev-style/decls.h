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
Res<P> parseDeclaration(Css::Sst const &sst) {
    if (sst != Css::Sst::DECL)
        panic("expected declaration");

    if (sst.token != Css::Token::IDENT)
        panic("expected ident");

    Res<P> resDecl = Error::invalidData("unknown declaration");

    P::any([&]<typename T>(Meta::Type<T>) -> bool {
        if (sst.token != Css::Token::ident(T::name()))
            return false;

        Cursor<Css::Sst> cursor = sst.content;

        auto res = parseDeclarationValue<T>(cursor);
        if (not res) {
            resDecl = res.none();
            return true;
        }

        resDecl = Ok(res.take());

        if constexpr (requires { P::important; }) {
            if (cursor.skip(Css::Token::delim("!")) and cursor.skip(Css::Token::ident("important")))
                resDecl.unwrap().important = Important::YES;
        }

        eatWhitespace(cursor);
        if (not cursor.ended()) {
            resDecl = Error::invalidData("unknown tokens in content");
        }

        return true;
    });

    if (not resDecl)
        logWarn("failed to parse declaration: {} - {}", sst, resDecl);

    return resDecl;
}

template <typename P>
Vec<P> parseDeclarations(Css::Content const &sst) {
    Vec<P> res;

    for (auto const &item : sst) {
        if (item != Css::Sst::DECL) {
            logWarn("unexpected item in declaration: {}", item.type);
            continue;
        }

        auto prop = parseDeclaration<P>(item);

        if (not prop) {
            logWarn("failed to parse declaration: {}", prop.none());
            continue;
        }
        res.pushBack(prop.take());
    }

    return res;
}

template <typename P>
Vec<P> parseDeclarations(Css::Sst const &sst) {
    return parseDeclarations<P>(sst.content);
}

template <typename P>
Vec<P> parseDeclarations(Str style) {
    Css::Lexer lex{style};
    auto sst = Css::consumeDeclarationList(lex, true);
    return parseDeclarations<P>(sst);
}

} // namespace Vaev::Style
