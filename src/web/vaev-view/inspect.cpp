#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>
#include <mdi/chevron-down.h>
#include <vaev-markup/dom.h>

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

Ui::Child elementStartTag(Markup::Element const &el) {
    return Ui::text(
        Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500),
        "<{}>", el.tagName
    );
}

Ui::Child elementEndTag(Markup::Element const &el) {
    return Ui::text(
        Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500),
        "</{}>", el.tagName
    );
}

Ui::Child itemHeader(Markup::Node const &n) {
    if (n.is<Markup::Document>()) {
        return Ui::codeMedium("#document");
    } else if (n.is<Markup::DocumentType>()) {
        return Ui::codeMedium("#document-type");
    } else if (auto tx = n.is<Markup::Text>()) {
        return Ui::codeMedium("#text {#}", tx->data);
    } else if (auto el = n.is<Markup::Element>()) {
        return Ui::hflow(n.children().len() ? Ui::icon(Mdi::CHEVRON_DOWN) : Ui::empty(), elementStartTag(*el));
    } else if (auto c = n.is<Markup::Comment>()) {
        return Ui::codeMedium(Gfx::GREEN, "<!-- {} -->", c->data);
    } else {
        unreachable();
    }
}

Ui::Child itemFooter(Markup::Node const &n, isize ident) {
    if (auto el = n.is<Markup::Element>()) {
        return Ui::hflow(n.children().len() ? guide() : Ui::empty(), elementEndTag(*el)) | idented(ident);
    }
    return Ui::empty();
}

Ui::Child item(Markup::Node const &n, isize ident) {
    return Ui::button(
        Ui::NOP,
        Ui::ButtonStyle::subtle(),
        itemHeader(n) | idented(ident)
    );
}

Ui::Child node(Markup::Node const &n, isize ident = 0) {
    Ui::Children children{item(n, ident)};
    for (auto &c : n.children()) {
        children.pushBack(node(*c, ident + 1));
    }
    children.pushBack(itemFooter(n, ident));
    return Ui::vflow(children);
}

Ui::Child inspect(Strong<Vaev::Markup::Document> dom) {
    return node(*dom);
}

} // namespace Vaev::View
