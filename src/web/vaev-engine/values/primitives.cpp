module;

#include <karm-core/macros.h>

export module Vaev.Engine:values.primitives;

import Karm.Core;
import Karm.Ref;

import :css;
import :values.base;

using namespace Karm;

namespace Vaev {

// MARK: Integer ---------------------------------------------------------------
// https://drafts.csswg.org/css-values/#integers

export using Integer = isize;

export template <>
struct ValueParser<Integer> {
    static Res<Integer> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            Integer result = try$(Io::atoi(scan));
            if (scan.ended())
                return Ok(result);
        }

        return Error::invalidData("expected integer");
    }
};

// MARK: Number ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#numbers

export using Number = f64;

export template <>
struct ValueParser<Number> {
    static Res<Number> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(try$(Io::atof(scan)));
        }

        return Error::invalidData("expected number");
    }
};

export template <>
struct ValueParser<bool> {
    static Res<bool> parse(Cursor<Css::Sst>& c) {
        return Ok(try$(parseValue<Integer>(c)) > 0);
    }
};

// MARK: String ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#strings

export template <>
struct ValueParser<String> {
    static Res<String> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::STRING) {
            // TODO: Handle escape sequences
            Io::SScan s = c.next().token.data.str();
            StringBuilder sb{s.rem()};
            auto quote = s.next();
            while (not s.skip(quote) and not s.ended()) {
                if (s.skip('\\') and not s.ended()) {
                    if (s.skip('\\'))
                        sb.append(s.next());
                } else {
                    sb.append(s.next());
                }
            }
            return Ok(sb.take());
        }

        return Error::invalidData("expected string");
    }
};

// MARK: Custom Ident ----------------------------------------------------------
// https://www.w3.org/TR/css-values-4/#identifier-value

struct CustomIdent {
    Symbol _symbol;
};

export template <>
struct ValueParser<CustomIdent> {
    static Res<CustomIdent> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (*c != Css::Token::IDENT)
            return Error::invalidData("expected custom ident");

        // TODO: Don't really know what to do with the following... lol
        //       Represents any valid CSS identifier that would not be
        //       misinterpreted as a pre-defined keyword in that property’s value definition
        //       (See https://www.w3.org/TR/css-values-4/#custom-idents)
        CustomIdent ident = CustomIdent{Symbol::from(c->token.data)};
        c.next();
        return Ok(std::move(ident));
    }
};

// MARK: Url -------------------------------------------------------------------
// https://www.w3.org/TR/css-values-4/#urls

export Res<String> parseUrlIntoString(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Css::Token::URL) {
        auto urlSize = c.peek().token.data.len() - 5; // "url()" takes 5 chars
        auto urlValue = sub(c.next().token.data, Range<usize>{4u, urlSize});
        return Ok(urlValue);
    } else if (c.peek() != Css::Sst::FUNC or c.peek().prefix != Css::Token::function("url("))
        return Error::invalidData("expected url function");

    auto urlFunc = c.next();
    Cursor<Css::Sst> scanUrl{urlFunc.content};
    eatWhitespace(scanUrl);

    if (scanUrl.ended() or not(scanUrl.peek() == Css::Token::STRING))
        return Error::invalidData("expected base url string");

    // TODO: it is unclear what url-modifiers are and how they are used
    return parseValue<String>(scanUrl);
}

export template <>
struct ValueParser<Ref::Url> {
    static Res<Ref::Url> parse(Cursor<Css::Sst>& c) {
        return Ok(Ref::parseUrlOrPath(try$(parseUrlIntoString(c))));
    }
};

export template <ValueParseable... Ts>
struct ValueParser<Union<Ts...>> {
    static Res<Union<Ts...>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        return Meta::any<Ts...>([&c]<typename T>() -> Res<Union<Ts...>> {
            return Ok(try$(parseValue<T>(c)));
        });
    }
};

} // namespace Vaev
