module;

#include <karm-logger/logger.h>
#include <karm-sys/file.h>

export module Vaev.Engine:loader.loader;

import Karm.Gc;
import Karm.Http;
import Karm.Core;
import Karm.Debug;
import Karm.Md;
import Karm.Ref;

import :dom;
import :html;
import :xml;
import :style;

namespace Vaev::Loader {

Async::Task<Gc::Ref<Dom::Document>> _loadDocumentAsync(Gc::Heap& heap, Ref::Url url, Rc<Http::Response> resp) {
    auto dom = heap.alloc<Dom::Document>(url);

    auto mime = resp->header.contentType();

    if (not mime.has())
        mime = Ref::sniffSuffix(url.path.suffix());

    if (not resp->body)
        co_return Error::invalidInput("response body is missing");

    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllUtf8Async(*respBody));

    if (not mime.has() or mime->is("application/octet-stream"_mime)) {
        mime = Ref::sniffBytes(bytes(buf));
        logWarn("{} has unspecified mime type, mime sniffing yielded '{}'", url, mime);
    }

    if (mime->is("text/html"_mime)) {
        Html::HtmlParser parser{heap, dom};
        parser.write(buf);

        co_return Ok(dom);
    } else if (mime->is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Xml::XmlParser parser{heap};
        co_try$(parser.parse(scan, Html::NAMESPACE, *dom));

        co_return Ok(dom);
    } else if (mime->is("image/svg+xml"_mime)) {
        Io::SScan scan{buf};
        Xml::XmlParser parser{heap};
        co_try$(parser.parse(scan, Svg::NAMESPACE, *dom));

        co_return Ok(dom);
    } else if (mime->is("text/markdown"_mime)) {
        auto doc = Md::parse(buf);
        logDebug("markdown: {}", doc);
        auto html = Md::renderHtml(doc);

        Html::HtmlParser parser{heap, dom};
        parser.write(html);

        co_return Ok(dom);
    } else if (mime->is("text/plain"_mime)) {
        auto text = heap.alloc<Dom::Text>();
        text->appendData(buf);

        auto body = heap.alloc<Dom::Element>(Html::BODY_TAG);
        body->appendChild(text);

        dom->appendChild(body);
        co_return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);

        co_return Error::invalidInput("unsupported MIME type");
    }
}

export Async::Task<Gc::Ref<Dom::Document>> viewSourceAsync(Gc::Heap& heap, Http::Client& client, Ref::Url const& url) {
    auto resp = co_trya$(client.getAsync(url));
    if (not resp->body)
        co_return Error::invalidInput("response body is missing");
    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllUtf8Async(*respBody));

    auto dom = heap.alloc<Dom::Document>(url);
    auto body = heap.alloc<Dom::Element>(Html::BODY_TAG);
    dom->appendChild(body);
    auto pre = heap.alloc<Dom::Element>(Html::PRE_TAG);
    body->appendChild(pre);
    auto text = heap.alloc<Dom::Text>(buf);
    pre->appendChild(text);

    co_return Ok(dom);
}

Async::Task<Style::StyleSheet> _fetchStylesheetAsync(Http::Client& client, Ref::Url url, Style::Origin origin) {
    auto resp = co_trya$(client.getAsync(url));

    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllUtf8Async(*respBody));

    Io::SScan s{buf};
    co_return Ok(Style::StyleSheet::parse(s, url, origin));
}

Async::Task<> _fetchStylesheetsAsync(Http::Client& client, Gc::Ref<Dom::Node> node, Style::StyleSheetList& sb) {
    auto el = node->is<Dom::Element>();
    if (el and el->qualifiedName == Html::STYLE_TAG) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        auto sheet = Style::StyleSheet::parse(textScan, node->baseURI());
        sb.add(std::move(sheet));
    } else if (el and el->qualifiedName == Html::LINK_TAG) {
        auto rel = el->getAttribute(Html::REL_ATTR);
        if (rel == "stylesheet"s) {
            auto href = el->getAttribute(Html::HREF_ATTR);
            if (not href) {
                logWarn("link element missing href attribute");
                co_return Error::invalidInput("link element missing href");
            }

            auto url = Ref::Url::resolveReference(node->baseURI(), Ref::parseUrlOrPath(*href));
            if (not url) {
                logWarn("failed to resolve stylesheet url: {}", url);
                co_return Error::invalidInput("failed to resolve stylesheet url");
            }

            auto sheet = co_await _fetchStylesheetAsync(client, url.unwrap(), Style::Origin::AUTHOR);
            if (not sheet) {
                logWarn("failed to fetch stylesheet from {}: {}", url, sheet);
                co_return Error::invalidInput("failed to fetch stylesheet from {}");
            }

            sb.add(sheet.take());
        }
    } else {
        for (auto child = node->firstChild(); child; child = child->nextSibling())
            (void)co_await _fetchStylesheetsAsync(client, *child, sb);
    }

    co_return Ok();
}

static Debug::Flag dumpDom{"web-dom"};
static Debug::Flag dumpStylesheets{"web-stylesheets"};

export Async::Task<Gc::Ref<Dom::Document>> fetchDocumentAsync(Gc::Heap& heap, Http::Client& client, Ref::Url const& url) {
    if (url.scheme == "about") {
        if (url.path.str() == "blank")
            co_return co_await fetchDocumentAsync(heap, client, "bundle://vaev-engine/blank.xhtml"_url);

        if (url.path.str() == "start")
            co_return co_await fetchDocumentAsync(heap, client, "bundle://vaev-engine/start-page.xhtml"_url);
    }

    auto resp = co_trya$(client.getAsync(url));
    auto dom = co_trya$(_loadDocumentAsync(heap, url, resp));
    auto stylesheets = heap.alloc<Style::StyleSheetList>();

    stylesheets->add((co_await _fetchStylesheetAsync(client, "bundle://vaev-engine/html.css"_url, Style::Origin::USER_AGENT))
                         .take("user agent stylesheet not available"));

    stylesheets->add((co_await _fetchStylesheetAsync(client, "bundle://vaev-engine/print.css"_url, Style::Origin::USER_AGENT))
                         .take("user agent stylesheet not available"));

    stylesheets->add((co_await _fetchStylesheetAsync(client, "bundle://vaev-engine/svg.css"_url, Style::Origin::USER_AGENT))
                         .take("user agent stylesheet not available"));

    (void)co_await _fetchStylesheetsAsync(client, *dom, *stylesheets);
    dom->styleSheets = stylesheets;

    if (dumpDom)
        logDebugIf(dumpDom, "document tree: {}", dom);

    if (dumpStylesheets)
        logDebugIf(dumpStylesheets, "document stylesheets: {}", stylesheets);

    co_return Ok(dom);
}

} // namespace Vaev::Loader
