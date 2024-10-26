#pragma once

#include <karm-ui/node.h>
#include <vaev-markup/dom.h>

namespace Vaev::View {

Ui::Child view(Strong<Markup::Document> dom);

} // namespace Vaev::View
