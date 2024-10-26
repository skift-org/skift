#pragma once

#include <karm-mime/mime.h>

#include "rules.h"

namespace Vaev::Style {

// https://www.w3.org/TR/cssom-1/#css-style-sheets
struct StyleSheet {
    Mime::Mime mime = "text/css"_mime;
    Mime::Url href = ""_url;
    Str title = "";
    Vec<Rule> rules;

    void repr(Io::Emit &e) const;

    static StyleSheet parse(Css::Sst const &sst);

    static Style::StyleSheet parse(Io::SScan &s) {
        Css::Lexer lex{s};
        Css::Sst sst = consumeRuleList(lex, true);
        return parse(sst);
    }
};

struct StyleBook {
    Vec<StyleSheet> styleSheets;

    void repr(Io::Emit &e) const;

    void add(StyleSheet &&sheet);
};

} // namespace Vaev::Style
