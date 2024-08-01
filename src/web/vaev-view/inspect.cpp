#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>
#include <vaev-dom/comment.h>
#include <vaev-dom/document-type.h>
#include <vaev-dom/element.h>
#include <vaev-dom/text.h>

#include "inspect.h"

namespace Vaev::View {

auto guide() {
    return Ui::hflow(
        Ui::empty(8),
        Ui::separator(),
        Ui::empty(9)
    );
}

auto idented(isize ident) {
    return [ident](Ui::Child c) -> Ui::Child {
        Ui::Children res;
        res.pushBack(Ui::empty(4));
        for (isize i = 0; i < ident; i++) {
            res.pushBack(guide());
        }
        res.pushBack(c);
        return Ui::hflow(res);
    };
}

Ui::Child elementStartTag(Dom::Element const &el) {
    return Ui::text(
        Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500),
        "<{}>", el.tagName
    );
}

Ui::Child elementEndTag(Dom::Element const &el) {
    return Ui::text(
        Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500),
        "</{}>", el.tagName
    );
}

Ui::Child itemHeader(Dom::Node const &n) {
    if (n.is<Dom::Document>()) {
        return Ui::codeMedium("#document");
    } else if (n.is<Dom::DocumentType>()) {
        return Ui::codeMedium("#document-type");
    } else if (auto *tx = n.is<Dom::Text>()) {
        return Ui::codeMedium("#text {#}", tx->data);
    } else if (auto *el = n.is<Dom::Element>()) {
        return Ui::hflow(n.children().len() ? Ui::icon(Mdi::CHEVRON_DOWN) : Ui::empty(), elementStartTag(*el));
    } else if (auto *c = n.is<Dom::Comment>()) {
        return Ui::codeMedium(Gfx::GREEN, "<!-- {} -->", c->data);
    } else {
        unreachable();
    }
}

Ui::Child itemFooter(Dom::Node const &n, isize ident) {
    if (auto *el = n.is<Dom::Element>()) {
        return Ui::hflow(n.children().len() ? guide() : Ui::empty(), elementEndTag(*el)) | idented(ident);
    }
    return Ui::empty();
}

Ui::Child item(Dom::Node const &n, isize ident) {
    return Ui::button(
        Ui::NOP,
        Ui::ButtonStyle::subtle(),
        itemHeader(n) | idented(ident)
    );
}

Ui::Child node(Dom::Node const &n, isize ident = 0) {
    Ui::Children children{item(n, ident)};
    for (auto &c : n.children()) {
        children.pushBack(node(*c, ident + 1));
    }
    children.pushBack(itemFooter(n, ident));
    return Ui::vflow(children);
}

Ui::Child inspect(Strong<Vaev::Dom::Document> dom) {
    return node(*dom);
}

} // namespace Vaev::View
