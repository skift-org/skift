#pragma once

#include <karm-mime/url.h>
#include <vaev-markup/dom.h>
#include <vaev-style/stylesheet.h>

namespace Vaev::Driver {

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url, Style::Origin origin = Style::Origin::AUTHOR);

Res<Strong<Markup::Document>> fetchDocument(Mime::Url const &url);

Res<Strong<Markup::Document>> loadDocument(Mime::Url const &url, Mime::Mime const &mime, Io::Reader &reader);

Res<Strong<Markup::Document>> viewSource(Mime::Url const &url);

} // namespace Vaev::Driver
