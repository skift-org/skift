#pragma once

#include <karm-ui/node.h>
#include <vaev-dom/document.h>

namespace Vaev::View {

Ui::Child view(Strong<Dom::Document> dom);

} // namespace Vaev::View
