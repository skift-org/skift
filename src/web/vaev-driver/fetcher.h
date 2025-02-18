#pragma once

#include <karm-gc/heap.h>
#include <karm-mime/url.h>
#include <vaev-dom/document.h>
#include <vaev-style/stylesheet.h>

namespace Vaev::Driver {

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url, Style::Origin origin = Style::Origin::AUTHOR);

void fetchStylesheets(Gc::Ref<Dom::Node> node, Style::StyleBook& sb);

Res<Gc::Ref<Dom::Document>> fetchDocument(Gc::Heap& heap, Mime::Url const& url);

Res<Gc::Ref<Dom::Document>> loadDocument(Gc::Heap& heap, Mime::Url const& url, Mime::Mime const& mime, Io::Reader& reader);

Res<Gc::Ref<Dom::Document>> viewSource(Gc::Heap& heap, Mime::Url const& url);

} // namespace Vaev::Driver
