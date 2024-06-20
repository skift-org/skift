#pragma once

#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-mime/mime.h>
#include <karm-mime/url.h>
#include <vaev-css/lexer.h>

#include "media.h"
#include "props.h"
#include "select.h"

namespace Vaev::Style {

struct Rule;

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
struct StyleRule {
    Selector selector;
    Vec<Prop> props;

    void repr(Io::Emit &e) const {
        e("(style-rule");
        e.indent();
        e("\nselector: {}", selector);
        if (props) {
            e.newline();
            e("props: [");
            e.indentNewline();
            for (auto const &prop : props) {
                e("{}\n", prop);
            }
            e.deindent();
            e("]\n");
        }
        e.deindent();
        e(")");
    }
};

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
struct ImportRule {
    Mime::Url url;

    void repr(Io::Emit &e) const {
        e("(import-rule {})", url);
    }
};

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
struct MediaRule {
    MediaQuery media;
    Vec<Rule> rules;

    void repr(Io::Emit &e) const {
        e("(media-rule {} {})", media, rules);
    }
};

// https://www.w3.org/TR/css-fonts-4/#cssfontfacerule
struct FontFaceRule {
    Vec<Prop> props;

    void repr(Io::Emit &e) const {
        e("(font-face-rule {})", props);
    }
};

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
using _Rule = Union<
    StyleRule,
    FontFaceRule,
    MediaRule,
    ImportRule>;

struct Rule : public _Rule {
    using _Rule::_Rule;

    void repr(Io::Emit &e) const {
        visit([&](auto const &r) {
            e("{}", r);
        });
    }
};

// https:// www.w3.org/TR/cssom-1/#css-style-sheets
struct StyleSheet {
    Mime::Mime mime = "text/css"_mime;
    Mime::Url href = ""_url;
    Str title = "";
    Vec<Rule> rules;

    void repr(Io::Emit &e) const {
        e("(style-sheet {} {} {}", mime, href, title);

        e.indent();
        if (rules) {
            e.newline();
            e("rules: [");
            e.indentNewline();
            for (auto const &rule : rules) {
                e("{}\n", rule);
            }
            e.deindent();
            e("]\n");
        }
        e.deindent();
        e(")");
    }
};

struct StyleBook {
    Vec<StyleSheet> styleSheets;

    void repr(Io::Emit &e) const {
        e("(style-book {})", styleSheets);
    }
};

} // namespace Vaev::Style
