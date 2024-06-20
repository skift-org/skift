#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <vaev-css>
#include <vaev-dom>
#include <vaev-html>
#include <vaev-layout>
#include <vaev-style>
#include <vaev-xml>

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

static void collectStyle(Dom::Node const &node, Style::StyleBook &sb) {
    if (auto *el = node.is<Dom::Element>(); el and el->tagName == Html::STYLE) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        auto sheet = Css::parseStylesheet(textScan);
        sb.add(std::move(sheet));
    } else {
        for (auto &child : node.children())
            collectStyle(*child, sb);
    }
}

void render(Dom::Document &dom) {
    logDebug("document: {}", dom);

    // Collect styles from the document
    Style::StyleBook stylebook;
    collectStyle(dom, stylebook);
    logDebug("stylebook: {}", stylebook);

    logDebug("building layout tree");
    Style::Computer computer{stylebook};
    Strong<Layout::Flow> root = makeStrong<Layout::BlockFlow>(makeStrong<Style::Computed>());
    Layout::build(computer, dom, *root);

    logDebug("computing layout");
    RectPx viewport{Px{800}, Px{600}};
    root->layout(viewport);
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
