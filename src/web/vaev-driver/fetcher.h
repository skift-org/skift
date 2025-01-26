#pragma once

#include <karm-mime/url.h>
#include <vaev-markup/dom.h>
#include <vaev-style/stylesheet.h>

namespace Vaev::Driver {

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url, Style::Origin origin = Style::Origin::AUTHOR);

void fetchStylesheets(Markup::Node const& node, Style::StyleBook& sb);

Res<Rc<Markup::Document>> fetchDocument(Mime::Url const& url);

Res<Rc<Markup::Document>> loadDocument(Mime::Url const& url, Mime::Mime const& mime, Io::Reader& reader);

Res<Rc<Markup::Document>> viewSource(Mime::Url const& url);

} // namespace Vaev::Driver
