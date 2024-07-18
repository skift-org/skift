#include <karm-mime/mime.h>
#include <karm-sys/file.h>
#include <vaev-html/parser.h>
#include <vaev-style/stylesheet.h>
#include <vaev-xml/parser.h>

#include "fetcher.h"

namespace Vaev::Driver {

Res<Strong<Dom::Document>> fetchDocument(Mime::Url url) {
    logInfo("fetching: {}", url);

    if (url.scheme == "about") {
        if (url.path.str() == "blank")
            return fetchDocument("bundle://hideo-browser/blank.xhtml"_url);

        if (url.path.str() == "start")
            return fetchDocument("bundle://hideo-browser/start-page.xhtml"_url);

        return Error::invalidInput("unsupported about page");
    }

    auto mime = Mime::sniffSuffix(url.path.suffix());

    if (not mime.has())
        return Error::invalidInput("cannot determine MIME type");

    auto dom = makeStrong<Dom::Document>();
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    if (mime->is("text/html"_mime)) {
        Html::Parser parser{dom};
        parser.write(buf);

        return Ok(dom);
    } else if (mime->is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Xml::Parser parser;
        dom = try$(parser.parse(scan, HTML));

        return Ok(dom);
    } else if (mime->is("image/svg+xml"_mime)) {
        Io::SScan scan{buf};
        Xml::Parser parser;
        dom = try$(parser.parse(scan, SVG));

        return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);
        return Error::invalidInput("unsupported MIME type");
    }
}

Res<Style::StyleSheet> fetchStylesheet(Mime::Url url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    return Ok(Style::StyleSheet::parse(s));
}

} // namespace Vaev::Driver
