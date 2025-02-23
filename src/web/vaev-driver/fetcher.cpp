module;

#include <karm-gc/heap.h>
#include <karm-mime/mime.h>
#include <karm-mime/url.h>
#include <karm-sys/dir.h>
#include <karm-sys/file.h>
#include <vaev-dom/document.h>
#include <vaev-dom/html/parser.h>
#include <vaev-dom/xml/parser.h>
#include <vaev-style/stylesheet.h>

export module Vaev.Driver:fetcher;

namespace Vaev::Driver {

export Res<Gc::Ref<Dom::Document>> loadDocument(Gc::Heap& heap, Mime::Url const& url, Mime::Mime const& mime, Io::Reader& reader) {
    auto dom = heap.alloc<Dom::Document>(url);
    auto buf = try$(Io::readAllUtf8(reader));

    if (mime.is("text/html"_mime)) {
        Dom::HtmlParser parser{heap, dom};
        parser.write(buf);

        return Ok(dom);
    } else if (mime.is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Dom::XmlParser parser{heap};
        try$(parser.parse(scan, HTML, *dom));

        return Ok(dom);
    } else if (mime.is("image/svg+xml"_mime)) {
        Io::SScan scan{buf};
        Dom::XmlParser parser{heap};
        try$(parser.parse(scan, SVG, *dom));

        return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);
        return Error::invalidInput("unsupported MIME type");
    }
}

export Res<Gc::Ref<Dom::Document>> viewSource(Gc::Heap& heap, Mime::Url const& url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    auto dom = heap.alloc<Dom::Document>(url);

    auto body = heap.alloc<Dom::Element>(Html::BODY);
    dom->appendChild(body);

    auto pre = heap.alloc<Dom::Element>(Html::PRE);
    body->appendChild(pre);

    auto text = heap.alloc<Dom::Text>(buf);
    pre->appendChild(text);

    return Ok(dom);
}

Res<Gc::Ref<Dom::Document>> indexOf(Gc::Heap& heap, Mime::Url const& url) {
    auto dom = heap.alloc<Dom::Document>(url);

    auto body = heap.alloc<Dom::Element>(Html::BODY);
    dom->appendChild(body);

    auto h1 = heap.alloc<Dom::Element>(Html::H1);
    body->appendChild(h1);

    auto text = heap.alloc<Dom::Text>(Io::format("Index of {}", url.path));
    h1->appendChild(text);

    auto ul = heap.alloc<Dom::Element>(Html::UL);
    body->appendChild(ul);

    auto dir = try$(Sys::Dir::open(url));

    for (auto const& entry : dir.entries()) {
        auto li = heap.alloc<Dom::Element>(Html::LI);
        ul->appendChild(li);

        auto a = heap.alloc<Dom::Element>(Html::A);
        li->appendChild(a);

        auto href = url.join(entry.name);
        a->setAttribute(Html::HREF_ATTR, href.str());

        auto text = heap.alloc<Dom::Text>(entry.name);
        a->appendChild(text);
    }

    return Ok(dom);
}

export Res<Gc::Ref<Dom::Document>> fetchDocument(Gc::Heap& heap, Mime::Url const& url) {
    if (url.scheme == "about") {
        if (url.path.str() == "blank")
            return fetchDocument(heap, "bundle://vaev-driver/blank.xhtml"_url);

        if (url.path.str() == "start")
            return fetchDocument(heap, "bundle://vaev-driver/start-page.xhtml"_url);

        return Error::invalidInput("unsupported about page");
    } else if (url.scheme == "file" or url.scheme == "bundle") {
        if (try$(Sys::isDir(url))) {
            return indexOf(heap, url);
        }

        auto mime = Mime::sniffSuffix(url.path.suffix());
        if (not mime.has())
            return Error::invalidInput("cannot determine MIME type");

        auto dom = makeRc<Dom::Document>(url);
        auto file = try$(Sys::File::open(url));
        return loadDocument(heap, url, *mime, file);
    } else {
        return Error::invalidInput("unsupported url scheme");
    }
}

export Res<Style::StyleSheet> fetchStylesheet(Mime::Url url, Style::Origin origin) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    return Ok(Style::StyleSheet::parse(s, origin));
}

export void fetchStylesheets(Gc::Ref<Dom::Node> node, Style::StyleBook& sb) {
    auto el = node->is<Dom::Element>();
    if (el and el->tagName == Html::STYLE) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        auto sheet = Style::StyleSheet::parse(textScan);
        sb.add(std::move(sheet));
    } else if (el and el->tagName == Html::LINK) {
        auto rel = el->getAttribute(Html::REL_ATTR);
        if (rel == "stylesheet"s) {
            auto href = el->getAttribute(Html::HREF_ATTR);
            if (not href) {
                logWarn("link element missing href attribute");
                return;
            }

            auto url = Mime::parseUrlOrPath(*href, node->baseURI());
            auto sheet = fetchStylesheet(url, Style::Origin::AUTHOR);
            if (not sheet) {
                logWarn("failed to fetch stylesheet from {}: {}", url, sheet);
                return;
            }

            sb.add(sheet.take());
        }
    } else {
        for (auto child = node->firstChild(); child; child = child->nextSibling())
            fetchStylesheets(*child, sb);
    }
}

} // namespace Vaev::Driver
