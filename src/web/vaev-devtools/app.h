#pragma once

#include <karm-ui/node.h>
#include <vaev-dom/document.h>

namespace Hideo::Browser {

Ui::Child app(Mime::Url url, Res<Strong<Vaev::Dom::Document>> dom);

} // namespace Hideo::Browser
