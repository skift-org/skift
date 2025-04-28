#include "stylesheet.h"

namespace Vaev::Style {

// MARK: StyleSheet ------------------------------------------------------------

void StyleSheet::repr(Io::Emit& e) const {
    e("(style-sheet {} {} {}", mime, href, title);

    e.indent();
    if (rules) {
        e.newline();
        e("rules: [");
        e.indentNewline();
        for (auto const& rule : rules) {
            e("{}\n", rule);
        }
        e.deindent();
        e("]\n");
    }
    e.deindent();
    e(")");
}

StyleSheet StyleSheet::parse(Css::Sst const& sst, Mime::Url href, Origin origin) {
    if (sst != Css::Sst::LIST)
        panic("expected list");

    StyleSheet res;
    for (auto const& item : sst.content) {
        if (item == Css::Sst::RULE) {
            res.rules.pushBack(Rule::parse(item, origin));
        } else {
            logWarn("unexpected item in stylesheet: {}", item.type);
        }
    }

    res.origin = origin;
    res.href = href;

    return res;
}

// MARK: StyleBook -------------------------------------------------------------

void StyleSheetList::repr(Io::Emit& e) const {
    e("(style-book {})", styleSheets);
}

void StyleSheetList::add(StyleSheet&& sheet) {
    styleSheets.pushBack(std::move(sheet));
}

} // namespace Vaev::Style
