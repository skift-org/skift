#pragma once

#include <karm-mime/url.h>
#include <vaev-dom/document.h>
#include <vaev-style/stylesheet.h>

namespace Vaev::Driver {

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url);

Res<Strong<Dom::Document>> fetchDocument(Mime::Url url);

} // namespace Vaev::Driver
