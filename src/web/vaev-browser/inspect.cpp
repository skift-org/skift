#include <karm-kira/resizable.h>
#include <karm-kira/side-panel.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>
#include <mdi/chevron-down.h>
#include <mdi/chevron-right.h>
#include <vaev-markup/dom.h>
#include <vaev-style/styles.h>

#include "inspect.h"

namespace Vaev::Browser {

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

Ui::Child elementStartTag(Markup::Element const& el, bool expanded) {
    return Ui::text(
        Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500),
        expanded ? "<{}>" : "<{}> … </{}>", el.tagName, el.tagName
    );
}

Ui::Child elementEndTag(Markup::Element const& el) {
    return Ui::text(
        Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500),
        "</{}>", el.tagName
    );
}

Ui::Child itemHeader(Rc<Markup::Node> n, Ui::Action<InspectorAction> a, bool expanded) {
    if (n.is<Markup::Document>()) {
        return Ui::codeMedium("#document");
    } else if (n.is<Markup::DocumentType>()) {
        return Ui::codeMedium("#document-type");
    } else if (auto tx = n.is<Markup::Text>()) {
        return Ui::codeMedium("{#}", tx->data);
    } else if (auto el = n.is<Markup::Element>()) {
        if (el->children().len()) {
            return Ui::hflow(
                Ui::icon(
                    expanded ? Mdi::CHEVRON_DOWN : Mdi::CHEVRON_RIGHT
                ) |
                    Ui::button(
                        [n, a](auto& btn) {
                            a(btn, ExpandNode{n});
                        },
                        Ui::ButtonStyle::subtle()
                    ),
                elementStartTag(*el, expanded)
            );
        } else {
            return elementStartTag(*el, false);
        }
    } else if (auto c = n.is<Markup::Comment>()) {
        return Ui::codeMedium(Gfx::GREEN, "<!-- {} -->", c->data);
    } else {
        unreachable();
    }
}

Ui::Child itemFooter(Rc<Markup::Node> n, isize ident) {
    if (auto el = n.is<Markup::Element>())
        return Ui::hflow(n->children().len() ? guide() : Ui::empty(), elementEndTag(*el)) | idented(ident);
    return Ui::empty();
}

Ui::ButtonStyle selected() {
    return {
        .idleStyle = {
            .backgroundFill = Ui::GRAY800,
            .foregroundFill = Ui::GRAY300,
        },
        .hoverStyle = {
            .borderWidth = 1,
            .backgroundFill = Ui::GRAY600,
        },
        .pressStyle = {
            .borderWidth = 1,
            .backgroundFill = Ui::GRAY700,
        },
    };
}

Ui::Child item(Rc<Markup::Node> n, InspectState const& s, Ui::Action<InspectorAction> a, bool expanded, isize ident) {
    auto style = s.selectedNode == n ? selected() : Ui::ButtonStyle::subtle().withRadii(0);
    return Ui::button(
        [n, a](auto& btn) {
            a(btn, SelectNode{n});
        },
        style,
        itemHeader(n, a, expanded) | idented(ident)
    );
}

Ui::Child node(Rc<Markup::Node> n, InspectState const& s, Ui::Action<InspectorAction> a, isize ident = 0) {
    bool expanded = n.is<Markup::Document>() or s.expandedNodes.has(n);
    Ui::Children children{item(n, s, a, expanded, ident)};

    if (expanded) {
        for (auto& c : n->children()) {
            children.pushBack(node(c, s, a, n.is<Markup::Document>() ? 0 : ident + 1));
        }
        children.pushBack(itemFooter(n, ident));
    }
    return Ui::vflow(children);
}

Ui::Child computedStyles() {
    Ui::Children children;

    Style::StyleProp::any([&]<typename T>(Meta::Type<T>) {
        if constexpr (requires { T::initial(); }) {
            children.pushBack(Ui::text(Ui::TextStyles::codeSmall(), "{}: {}", T::name(), T::initial()) | Ui::insets({4, 8}));
        }
    });

    return Ui::vflow(
               Kr::sidePanelTitle("Computed Styles") | Ui::dragRegion({0, -1}),
               Ui::separator(),
               Ui::vflow(children) | Ui::vscroll() | Ui::grow()
           ) |
           Ui::pinSize(128);
}

Ui::Child inspect(Rc<Vaev::Markup::Document> n, InspectState const& s, Ui::Action<InspectorAction> a) {
    return Ui::vflow(
        node(n, s, a) | Ui::vscroll() | Ui::grow(),
        computedStyles() | Kr::resizable(Kr::ResizeHandle::TOP, {128}, NONE)
    );
}

} // namespace Vaev::Browser
