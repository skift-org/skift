#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "inspect.h"

namespace Web::View {

Ui::Child inspectNode(Strong<Dom::Node> node) {
    Ui::Children children;
    if (node->hasChildren()) {
        for (auto child : node->_children)
            children.pushBack(inspectNode(child));
    }

    return Ui::vflow(
        Ui::text(Dom::toStr(node->nodeType())),
        Ui::vflow(children) | Ui::spacing({16, 0, 0, 0})
    );
}

Ui::Child inspect(Strong<Dom::Document> dom) {
    return Ui::vflow(
        Ui::labelLarge("Inspect"),
        inspectNode(dom) |
            Ui::vscroll() |
            Ui::pinSize({200, 0}) |
            Ui::grow()
    );
}

} // namespace Web::View
