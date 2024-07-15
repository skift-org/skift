#pragma once

#include <vaev-css/parser.h>

namespace Vaev::Style {

template <typename T>
Res<T> parsePropertyValue(Cursor<Css::Sst> &c) {
    if constexpr (requires { T{}.parse(c); }) {
        T t;
        try$(t.parse(c));
        return Ok(std::move(t));
    } else {
        logError("missing parser for property: {}", T::name());
        return Error::notImplemented("missing parser for property");
    }
}

template <typename P>
Res<P> parseProperty(Css::Sst const &sst) {
    if (sst != Css::Sst::DECL)
        panic("expected declaration");

    if (sst.token != Css::Token::IDENT)
        panic("expected ident");

    Res<P> resProp = Error::invalidData("unknown property");

    P::any([&]<typename T>(Meta::Type<T>) -> bool {
        if (sst != Css::Token::ident(T::name()))
            return false;
        Cursor<Css::Sst> cursor = sst.content;
        auto res = parsePropertyValue<T>(cursor);
        if (not res)
            return false;

        resProp = Ok(res.take());
        return true;
    });

    if (not resProp)
        logWarn("failed to parse property: {} - {}", sst, resProp);

    return resProp;
}

template <typename P>
Vec<P> parseProperties(Css::Sst const &sst) {
    Vec<P> res;

    for (auto const &item : sst.content) {
        if (item != Css::Sst::DECL) {
            logWarn("unexpected item in properties: {}", item.type);
            continue;
        }

        auto prop = parseProperty<P>(item);

        if (not prop) {
            logWarn("failed to parse property: {}", prop.none());
            continue;
        }
        res.pushBack(prop.take());
    }

    return res;
}

} // namespace Vaev::Style
