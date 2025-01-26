#include <karm-mime/mime.h>
#include <karm-sys/dir.h>
#include <karm-sys/file.h>
#include <vaev-markup/html.h>
#include <vaev-markup/xml.h>
#include <vaev-style/stylesheet.h>

#include "fetcher.h"

namespace Vaev::Driver {

Res<Rc<Markup::Document>> loadDocument(Mime::Url const& url, Mime::Mime const& mime, Io::Reader& reader) {
    auto dom = makeRc<Markup::Document>(url);
    auto buf = try$(Io::readAllUtf8(reader));

    if (mime.is("text/html"_mime)) {
        Markup::HtmlParser parser{dom};
        parser.write(buf);

        return Ok(dom);
    } else if (mime.is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Markup::XmlParser parser;
        try$(parser.parse(scan, HTML, *dom));

        return Ok(dom);
    } else if (mime.is("image/svg+xml"_mime)) {
        Io::SScan scan{buf};
        Markup::XmlParser parser;
        try$(parser.parse(scan, SVG, *dom));

        return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);
        return Error::invalidInput("unsupported MIME type");
    }
}

Res<Rc<Markup::Document>> viewSource(Mime::Url const& url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    auto dom = makeRc<Markup::Document>(url);

    auto body = makeRc<Markup::Element>(Html::BODY);
    dom->appendChild(body);

    auto pre = makeRc<Markup::Element>(Html::PRE);
    body->appendChild(pre);

    auto text = makeRc<Markup::Text>(buf);
    pre->appendChild(text);

    return Ok(dom);
}

Res<Rc<Markup::Document>> indexOf(Mime::Url const& url) {
    auto dom = makeRc<Markup::Document>(url);

    auto body = makeRc<Markup::Element>(Html::BODY);
    dom->appendChild(body);

    auto h1 = makeRc<Markup::Element>(Html::H1);
    body->appendChild(h1);

    auto text = makeRc<Markup::Text>(Io::format("Index of {}", url.path).unwrapOr(""s));
    h1->appendChild(text);

    auto ul = makeRc<Markup::Element>(Html::UL);
    body->appendChild(ul);

    auto dir = try$(Sys::Dir::open(url));

    for (auto const& entry : dir.entries()) {
        auto li = makeRc<Markup::Element>(Html::LI);
        ul->appendChild(li);

        auto a = makeRc<Markup::Element>(Html::A);
        li->appendChild(a);

        auto href = url.join(entry.name);
        a->setAttribute(Html::HREF_ATTR, href.str());

        auto text = makeRc<Markup::Text>(entry.name);
        a->appendChild(text);
    }

    return Ok(dom);
}

Res<Rc<Markup::Document>> fetchDocument(Mime::Url const& url) {
    if (url.scheme == "about") {
        if (url.path.str() == "blank")
            return fetchDocument("bundle://vaev-driver/blank.xhtml"_url);

        if (url.path.str() == "start")
            return fetchDocument("bundle://vaev-driver/start-page.xhtml"_url);

        return Error::invalidInput("unsupported about page");
    } else if (url.scheme == "file" or url.scheme == "bundle") {
        if (try$(Sys::isDir(url))) {
            return indexOf(url);
        } else {
            auto mime = Mime::sniffSuffix(url.path.suffix());

            if (not mime.has())
                return Error::invalidInput("cannot determine MIME type");

            auto dom = makeRc<Markup::Document>(url);
            auto file = try$(Sys::File::open(url));
            return loadDocument(url, *mime, file);
        }
    } else {
        return Error::invalidInput("unsupported url scheme");
    }
}

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url, Style::Origin origin) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    return Ok(Style::StyleSheet::parse(s, origin));
}

void fetchStylesheets(Markup::Node const& node, Style::StyleBook& sb) {
    auto el = node.is<Markup::Element>();
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

            auto url = Mime::parseUrlOrPath(*href);
            if (not url) {
                logWarn("link element href attribute is not a valid URL: {}", *href);
                return;
            }

            auto sheet = fetchStylesheet(url.take(), Style::Origin::AUTHOR);
            if (not sheet) {
                logWarn("failed to fetch stylesheet: {}", sheet);
                return;
            }

            sb.add(sheet.take());
        }
    } else {
        for (auto& child : node.children())
            fetchStylesheets(*child, sb);
    }
}

} // namespace Vaev::Driver
