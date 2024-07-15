#include <karm-sys/file.h>

#include "builder.h"
#include "parse.h"
#include "parser.h"
#include "selectors.h"

namespace Vaev::Css {

// https://www.w3.org/TR/css-syntax-3/#parse-stylesheet

Style::StyleSheet parseStylesheet(Io::SScan &s) {
    Lexer lex{s};
    Sst sst = consumeRuleList(lex, true);
    return parseStyleSheet(sst);
}

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    return Ok(parseStylesheet(s));
}

Style::Selector parseSelector(Io::SScan &s) {
    auto lex = Lexer{s};
    auto val = consumeSelector(lex);
    return parseSelector(val);
};

Style::Selector parseSelector(Str input) {
    Io::SScan s{input};
    return parseSelector(s);
};

} // namespace Vaev::Css
