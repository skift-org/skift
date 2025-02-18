#pragma once

#include <karm-ui/node.h>
#include <karm-ui/reducer.h>
#include <vaev-dom/document.h>

namespace Vaev::Browser {

struct ExpandNode {
    Gc::Ref<Dom::Node> node;
};

struct SelectNode {
    Gc::Ref<Dom::Node> node;
};

using InspectorAction = Union<ExpandNode, SelectNode>;

struct InspectState {
    Map<Gc::Ref<Dom::Node>, bool> expandedNodes = {};
    Opt<Gc::Ref<Dom::Node>> selectedNode = NONE;

    void apply(InspectorAction& a) {
        a.visit(Visitor{
            [&](ExpandNode e) {
                if (expandedNodes.has(e.node))
                    expandedNodes.del(e.node);
                else
                    expandedNodes.put(e.node, true);
            },
            [&](SelectNode e) {
                if (e.node->hasChildren())
                    expandedNodes.put(e.node, true);
                selectedNode = e.node;
            },
        });
    }
};

Ui::Child inspect(Gc::Ref<Vaev::Dom::Document> dom, InspectState const& s, Ui::Action<InspectorAction> action);

} // namespace Vaev::Browser
