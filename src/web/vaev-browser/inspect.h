#pragma once

#include <karm-ui/node.h>
#include <karm-ui/reducer.h>
#include <vaev-markup/dom.h>

namespace Vaev::Browser {

struct ExpandNode {
    Strong<Markup::Node> node;
};

struct SelectNode {
    Strong<Markup::Node> node;
};

using InspectorAction = Union<ExpandNode, SelectNode>;

struct InspectState {
    Map<Strong<Markup::Node>, bool> expandedNodes = {};
    Opt<Strong<Markup::Node>> selectedNode = NONE;

    void apply(InspectorAction &a) {
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

Ui::Child inspect(Strong<Vaev::Markup::Document> dom, InspectState const &s, Ui::Action<InspectorAction> action);

} // namespace Vaev::Browser
