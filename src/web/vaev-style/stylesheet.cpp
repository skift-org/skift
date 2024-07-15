#include "stylesheet.h"

namespace Vaev::Style {

// MARK: StyleSheet ------------------------------------------------------------

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

StyleSheet StyleSheet::parse(Css::Sst const &sst) {
    if (sst != Css::Sst::LIST)
        panic("expected list");

    Style::StyleSheet res;
    for (auto const &item : sst.content) {
        if (item == Css::Sst::RULE) {
            res.rules.pushBack(Rule::parse(item));
        } else {
            logWarn("unexpected item in stylesheet: {}", item.type);
        }
    }

    return res;
}

// MARK: StyleBook -------------------------------------------------------------

void StyleBook::repr(Io::Emit &e) const {
    e("(style-book {})", styleSheets);
}

void StyleBook::add(StyleSheet &&sheet) {
    styleSheets.pushBack(std::move(sheet));
}

} // namespace Vaev::Style
