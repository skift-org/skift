#pragma once

#include <karm-ui/node.h>
#include <karm-ui/reducer.h>
#include <vaev-markup/dom.h>

namespace Vaev::Browser {

struct ExpandNode {
    Rc<Markup::Node> node;
};

struct SelectNode {
    Rc<Markup::Node> node;
};

using InspectorAction = Union<ExpandNode, SelectNode>;

struct InspectState {
    Map<Rc<Markup::Node>, bool> expandedNodes = {};
    Opt<Rc<Markup::Node>> selectedNode = NONE;

    void apply(InspectorAction& a) {
        a.visit(Visitor{
            [&](ExpandNode e) {
                if (expandedNodes.has(e.node))
                    expandedNodes.del(e.node);
                else
                    expandedNodes.put(e.node, true);
            },
            [&](SelectNode e) {
                if (e.node->children())
                    expandedNodes.put(e.node, true);
                selectedNode = e.node;
            },
        });
    }
};

Ui::Child inspect(Rc<Vaev::Markup::Document> dom, InspectState const& s, Ui::Action<InspectorAction> action);

} // namespace Vaev::Browser
