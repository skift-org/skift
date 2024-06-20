#pragma once

#include <karm-ui/node.h>
#include <vaev-dom/document.h>

namespace Hideo::Browser {

Ui::Child app(Mime::Url url, Strong<Vaev::Dom::Document> dom, Opt<Error> err);

Res<Strong<Vaev::Dom::Document>> fetch(Mime::Url url);

} // namespace Hideo::Browser
