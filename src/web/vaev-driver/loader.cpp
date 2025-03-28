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

export module Vaev.Driver:loader;

import Karm.Http;
import Karm.Aio;

namespace Vaev::Driver {

Async::Task<Gc::Ref<Dom::Document>> _loadDocumentAsync(Gc::Heap& heap, Mime::Url url, Rc<Http::Response> resp) {
    auto dom = heap.alloc<Dom::Document>(url);

    auto mime = resp->header.contentType();

    if (not mime.has())
        mime = Mime::sniffSuffix(url.path.suffix());

    if (not resp->body)
        co_return Error::invalidInput("response body is missing");

    if (not mime.has() and url == "fd:stdin"_url) {
        mime = "text/html"_mime;
        logInfo("assuming stdin is {}", mime);
    }

    if (not mime.has())
        co_return Error::invalidInput("cannot determine MIME type");

    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllUtf8Async(*respBody));

    if (mime->is("text/html"_mime)) {
        Dom::HtmlParser parser{heap, dom};
        parser.write(buf);

        co_return Ok(dom);
    } else if (mime->is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Dom::XmlParser parser{heap};
        co_try$(parser.parse(scan, HTML, *dom));

        co_return Ok(dom);
    } else if (mime->is("image/svg+xml"_mime)) {
        Io::SScan scan{buf};
        Dom::XmlParser parser{heap};
        co_try$(parser.parse(scan, SVG, *dom));

        co_return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);

        co_return Error::invalidInput("unsupported MIME type");
    }
}

export Async::Task<Gc::Ref<Dom::Document>> viewSourceAsync(Gc::Heap& heap, Http::Client& client, Mime::Url const& url) {
    auto resp = co_trya$(client.getAsync(url));
    if (not resp->body)
        co_return Error::invalidInput("response body is missing");
    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllUtf8Async(*respBody));

    auto dom = heap.alloc<Dom::Document>(url);
    auto body = heap.alloc<Dom::Element>(Html::BODY);
    dom->appendChild(body);
    auto pre = heap.alloc<Dom::Element>(Html::PRE);
    body->appendChild(pre);
    auto text = heap.alloc<Dom::Text>(buf);
    pre->appendChild(text);

    co_return Ok(dom);
}

export Async::Task<Gc::Ref<Dom::Document>> fetchDocumentAsync(Gc::Heap& heap, Http::Client& client, Mime::Url const& url) {
    if (url.scheme == "about") {
        if (url.path.str() == "blank")
            co_return co_await fetchDocumentAsync(heap, client, "bundle://vaev-driver/blank.xhtml"_url);

        if (url.path.str() == "start")
            co_return co_await fetchDocumentAsync(heap, client, "bundle://vaev-driver/start-page.xhtml"_url);
    }

    auto resp = co_trya$(client.getAsync(url));
    co_return co_await _loadDocumentAsync(heap, url, resp);
}

export Res<Style::StyleSheet> fetchStylesheet(Mime::Url url, Style::Origin origin) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    return Ok(Style::StyleSheet::parse(s, url, origin));
}

export void fetchStylesheets(Gc::Ref<Dom::Node> node, Style::StyleBook& sb) {
    auto el = node->is<Dom::Element>();
    if (el and el->tagName == Html::STYLE) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        auto sheet = Style::StyleSheet::parse(textScan, node->baseURI());
        sb.add(std::move(sheet));
    } else if (el and el->tagName == Html::LINK) {
        auto rel = el->getAttribute(Html::REL_ATTR);
        if (rel == "stylesheet"s) {
            auto href = el->getAttribute(Html::HREF_ATTR);
            if (not href) {
                logWarn("link element missing href attribute");
                return;
            }

            auto url = Mime::Url::resolveReference(node->baseURI(), Mime::parseUrlOrPath(*href));
            if (not url) {
                logWarn("failed to resolve stylesheet url: {}", url);
                return;
            }

            auto sheet = fetchStylesheet(url.unwrap(), Style::Origin::AUTHOR);
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
