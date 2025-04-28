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
    Origin origin = Origin::AUTHOR;

    void repr(Io::Emit& e) const;

    static StyleSheet parse(Css::Sst const& sst, Mime::Url href, Origin origin = Origin::AUTHOR);

    static StyleSheet parse(Io::SScan& s, Mime::Url href, Origin origin = Origin::AUTHOR) {
        Css::Lexer lex{s};
        Css::Sst sst = consumeRuleList(lex, true);
        return parse(sst, href, origin);
    }

    void add(Rule&& rule) {
        rules.pushBack(std::move(rule));
    }
};

struct StyleSheetList {
    Vec<StyleSheet> styleSheets;

    void repr(Io::Emit& e) const;

    void add(StyleSheet&& sheet);
};

} // namespace Vaev::Style
