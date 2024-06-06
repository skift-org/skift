#include "mod.h"

#include "parser.h"

namespace Web::Css {

// https://www.w3.org/TR/css-syntax-3/#parse-stylesheet

Res<CSSOM::StyleSheet> parseStylesheet(Io::SScan &s) {
    Sst sst = try$(consumeRuleList(s));
    return Ok(CSSOM::StyleSheet{.cssRules = parseSST(sst)});
}

Res<CSSOM::StyleSheet> fetchStylesheet(Mime::Url url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    return parseStylesheet(s);
}

} // namespace Web::Css
