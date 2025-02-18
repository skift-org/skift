#pragma once

#include <karm-gc/root.h>
#include <karm-ui/node.h>
#include <vaev-dom/document.h>

namespace Vaev::View {

struct ViewProps {
    bool wireframe = false;
};

Ui::Child view(Gc::Root<Dom::Document> dom, ViewProps props);

} // namespace Vaev::View
