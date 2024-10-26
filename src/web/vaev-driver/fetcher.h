#pragma once

#include <karm-mime/url.h>
#include <vaev-markup/dom.h>
#include <vaev-style/stylesheet.h>

namespace Vaev::Driver {

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url);

Res<Strong<Markup::Document>> fetchDocument(Mime::Url const &url);

Res<Strong<Markup::Document>> loadDocument(Mime::Url const &url, Mime::Mime const &mime, Io::Reader &reader);

} // namespace Vaev::Driver
