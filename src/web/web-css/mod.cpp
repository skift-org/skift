#include "mod.h"

#include "parser.h"

namespace Web::Css {

// https://www.w3.org/TR/css-syntax-3/#parse-stylesheet

CSSOM::StyleSheet parseStylesheet(Io::SScan &s) {
    Lexer lex{s};
    Sst sst = consumeRuleList(lex);
    return CSSOM::StyleSheet{.rules = parseSST(sst)};
}

Res<CSSOM::StyleSheet> fetchStylesheet(Mime::Url url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    return Ok(parseStylesheet(s));
}

} // namespace Web::Css
