export module Vaev.Engine:values.base;

import Karm.Core;
import :css;

using namespace Karm;

namespace Vaev {

export void eatWhitespace(Cursor<Css::Sst>& c) {
    while (not c.ended() and c.peek() == Css::Token::WHITESPACE)
        c.next();
}

// https://www.w3.org/TR/css-values-4/#comb-comma
export bool skipOmmitableComma(Cursor<Css::Sst>& c) {
    eatWhitespace(c);
    bool res = c.skip(Css::Token::COMMA);
    eatWhitespace(c);
    return res;
}

export template <typename T>
struct ValueParser;

export template <typename T>
Res<T> parseValue(Cursor<Css::Sst>& c) {
    return ValueParser<T>::parse(c);
}

export template <typename T>
Res<T> parseValue(Str str) {
    Css::Lexer lex{str};
    auto content = Css::consumeDeclarationValue(lex);
    Cursor<Css::Sst> c{content};
    return ValueParser<T>::parse(c);
}

export template <typename T>
concept ValueParseable = requires(T a, Cursor<Css::Sst> c) {
    parseValue<T>(c);
};

} // namespace Vaev
