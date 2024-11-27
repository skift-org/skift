#include <karm-mime/mime.h>
#include <karm-sys/dir.h>
#include <karm-sys/file.h>
#include <vaev-markup/html.h>
#include <vaev-markup/xml.h>
#include <vaev-style/stylesheet.h>

#include "fetcher.h"

namespace Vaev::Driver {

Res<Strong<Markup::Document>> loadDocument(Mime::Url const &, Mime::Mime const &mime, Io::Reader &reader) {
    auto dom = makeStrong<Markup::Document>();
    auto buf = try$(Io::readAllUtf8(reader));

    if (mime.is("text/html"_mime)) {
        Markup::HtmlParser parser{dom};
        parser.write(buf);

        return Ok(dom);
    } else if (mime.is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Markup::XmlParser parser;
        dom = try$(parser.parse(scan, HTML));

        return Ok(dom);
    } else if (mime.is("image/svg+xml"_mime)) {
        Io::SScan scan{buf};
        Markup::XmlParser parser;
        dom = try$(parser.parse(scan, SVG));

        return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);
        return Error::invalidInput("unsupported MIME type");
    }
}

Res<Strong<Markup::Document>> viewSource(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    auto dom = makeStrong<Markup::Document>();

    auto body = makeStrong<Markup::Element>(Html::BODY);
    dom->appendChild(body);

    auto pre = makeStrong<Markup::Element>(Html::PRE);
    body->appendChild(pre);

    auto text = makeStrong<Markup::Text>(buf);
    pre->appendChild(text);

    return Ok(dom);
}

Res<Strong<Markup::Document>> indexOf(Mime::Url const &url) {
    auto dom = makeStrong<Markup::Document>();

    auto body = makeStrong<Markup::Element>(Html::BODY);
    dom->appendChild(body);

    auto h1 = makeStrong<Markup::Element>(Html::H1);
    body->appendChild(h1);

    auto text = makeStrong<Markup::Text>(Io::format("Index of {}", url.path).unwrapOr(""s));
    h1->appendChild(text);

    auto ul = makeStrong<Markup::Element>(Html::UL);
    body->appendChild(ul);

    auto dir = try$(Sys::Dir::open(url));

    for (auto const &entry : dir.entries()) {
        auto li = makeStrong<Markup::Element>(Html::LI);
        ul->appendChild(li);

        auto a = makeStrong<Markup::Element>(Html::A);
        li->appendChild(a);

        auto href = url.join(entry.name);
        a->setAttribute(Html::HREF_ATTR, href.str());

        auto text = makeStrong<Markup::Text>(entry.name);
        a->appendChild(text);
    }

    return Ok(dom);
}

Res<Strong<Markup::Document>> fetchDocument(Mime::Url const &url) {
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

            auto dom = makeStrong<Markup::Document>();
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

} // namespace Vaev::Driver
