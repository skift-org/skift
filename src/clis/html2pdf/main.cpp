#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <vaev-dom/document.h>
#include <vaev-html/parser.h>
#include <vaev-style/stylesheet.h>
#include <vaev-xml/parser.h>

namespace Vaev {

Res<Strong<Vaev::Dom::Document>> fetch(Mime::Url url) {
    logInfo("fetching: {}", url);

    if (url.scheme == "about") {
        if (url.path.str() == "./blank")
            return fetch("bundle://hideo-browser/blank.xhtml"_url);

        if (url.path.str() == "./start")
            return fetch("bundle://hideo-browser/start-page.xhtml"_url);

        return Error::invalidInput("unsupported about page");
    }

    auto mime = Mime::sniffSuffix(url.path.suffix());

    if (not mime.has())
        return Error::invalidInput("cannot determine MIME type");

    auto dom = makeStrong<Vaev::Dom::Document>();
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    if (mime->is("text/html"_mime)) {
        Vaev::Html::Parser parser{dom};
        parser.write(buf);

        return Ok(dom);
    } else if (mime->is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Vaev::Xml::Parser parser;
        dom = try$(parser.parse(scan, Vaev::HTML));

        return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);
        return Error::invalidInput("unsupported MIME type");
    }
}

static void collectStyle(Dom::Node &, Style::StyleBook &) {
}

void render(Dom::Document &dom) {
    logDebug("document: {}", dom);

    Style::StyleBook stylebook;
    collectStyle(dom, stylebook);
}

} // namespace Vaev

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);
    if (args.len() != 2) {
        Sys::errln("usage: html2pdf <input.html> <output.pdf>\n");
        co_return Error::invalidInput();
    }

    auto input = co_try$(Mime::parseUrlOrPath(args[0]));
    auto dom = co_try$(Vaev::fetch(input));

    Vaev::render(*dom);

    co_return Ok();
}
