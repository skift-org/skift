#include "stylesheet.h"

namespace Vaev::Style {

void StyleRule::repr(Io::Emit &e) const {
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

void ImportRule::repr(Io::Emit &e) const {
    e("(import-rule {})", url);
}

void MediaRule::repr(Io::Emit &e) const {
    e("(media-rule");
    e.indent();
    e("\nmedia: {}", media);
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
}

bool MediaRule::match(Media const &m) const {
    return media.match(m);
}

void FontFaceRule::repr(Io::Emit &e) const {
    e("(font-face-rule {})", props);
}

void Rule::repr(Io::Emit &e) const {
    visit([&](auto const &r) {
        e("{}", r);
    });
}

void StyleSheet::repr(Io::Emit &e) const {
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

void StyleBook::repr(Io::Emit &e) const {
    e("(style-book {})", styleSheets);
}

void StyleBook::add(StyleSheet &&sheet) {
    styleSheets.pushBack(std::move(sheet));
}

} // namespace Vaev::Style
