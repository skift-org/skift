#pragma once

#include <karm-ui/node.h>
#include <vaev-markup/dom.h>

namespace Vaev::View {

struct ViewProps {
    bool wireframe = false;
};

Ui::Child view(Rc<Markup::Document> dom, ViewProps props);

} // namespace Vaev::View
