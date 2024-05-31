#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>
#include <web-dom/element.h>
#include <web-dom/text.h>

#include "inspect.h"

namespace Web::View {

Ui::Child element(Strong<Dom::Element> el);

Ui::Child document(Strong<Dom::Document> doc);

// MARK: Node ------------------------------------------------------------------

Ui::Child node(Strong<Dom::Node> node) {
    if (node->nodeType() == Dom::NodeType::DOCUMENT) {
        auto el = *node.cast<Dom::Document>();
        return document(el);
    } else if (node->nodeType() == Dom::NodeType::ELEMENT) {
        auto el = *node.cast<Dom::Element>();
        return element(el);
    } else if (node->nodeType() == Dom::NodeType::TEXT) {
        auto el = *node.cast<Dom::Text>();
        return Ui::codeSmall("#text {#}", el->data);
    } else {
        return Ui::codeSmall("#node");
    }
}

Ui::Child nodeChildren(Strong<Dom::Node> n) {
    Ui::Children children;

    for (auto child : n->_children)
        children.pushBack(node(child));

    return vflow(children);
}

// MARK: Element ---------------------------------------------------------------

Ui::Child elementStartTag(Strong<Dom::Element> el) {
    return Ui::text(
        Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500),
        "<{}>", el->tagName
    );
}

Ui::Child elementEndTag(Strong<Dom::Element> el) {
    return Ui::text(
        Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500),
        "</{}>", el->tagName
    );
}

Ui::Child elementBody(Strong<Dom::Element> el, bool expanded) {
    if (not expanded) {
        return Ui::hflow(elementStartTag(el), Ui::icon(Mdi::DOTS_HORIZONTAL_CIRCLE_OUTLINE, Ui::GRAY400), elementEndTag(el));
    }

    return Ui::vflow(
        elementStartTag(el),
        nodeChildren(el),
        elementEndTag(el)
    );
}

Ui::Child element(Strong<Dom::Element> el) {
    return Ui::state<bool>(false, [el](auto expanded, auto bind) {
        return Ui::hflow(
            Ui::button(bind(not expanded), Ui::ButtonStyle::subtle(), Ui::icon(expanded ? Mdi::CHEVRON_DOWN : Mdi::CHEVRON_RIGHT, 18)),
            elementBody(el, expanded)
        );
    });
}

// MARK: Document --------------------------------------------------------------

Ui::Child document(Strong<Dom::Document> doc) {
    return Ui::vflow(
        Ui::codeSmall("#document"),
        nodeChildren(doc)
    );
}

Ui::Child inspect(Strong<Dom::Document> dom) {
    return Ui::vflow(
               Ui::hflow(
                   Ui::labelLarge("Inspector"),
                   Ui::grow(NONE),
                   Ui::button(
                       Ui::NOP,
                       Ui::ButtonStyle::subtle(),
                       Ui::icon(Mdi::CLOSE) | Ui::center()
                   )
               ) | Ui::spacing(6),
               Ui::separator(),
               Ui::vflow(node(dom)) |
                   Ui::spacing(6) |
                   Ui::vscroll() |
                   Ui::grow()
           ) |
           Ui::pinSize({320, 0});
}

} // namespace Web::View
