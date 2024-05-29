#pragma once

#include <karm-ui/node.h>
#include <web-dom/document.h>

namespace Hideo::Browser {

Ui::Child app(Mime::Url url, Strong<Web::Dom::Document> dom, Opt<Error> err);

Res<Strong<Web::Dom::Document>> fetch(Mime::Url url);

} // namespace Hideo::Browser
