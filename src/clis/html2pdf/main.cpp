#include <karm-mime/mime.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/time.h>
#include <vaev-html/parser.h>
#include <vaev-view/render.h>
#include <vaev-xml/parser.h>

namespace Vaev {

Style::Media constructMedia(Vec2Px pageSize) {
    return {
        .type = MediaType::SCREEN,
        .width = Px{pageSize.width},
        .height = Px{pageSize.height},
        .aspectRatio = pageSize.width.toFloat<Number>() / pageSize.height.toFloat<Number>(),
        .orientation = Orientation::LANDSCAPE,

        .resolution = Resolution::fromDpi(96),
        .scan = Scan::PROGRESSIVE,
        .grid = false,
        .update = Update::FAST,

        .overflowBlock = OverflowBlock::SCROLL,
        .overflowInline = OverflowInline::SCROLL,

        .color = 8,
        .colorIndex = 0,
        .monochrome = 0,
        .colorGamut = ColorGamut::SRGB,
        .pointer = Pointer::FINE,
        .hover = Hover::HOVER,
        .anyPointer = Pointer::FINE,
        .anyHover = Hover::HOVER,
    };
}

Res<Strong<Dom::Document>>
fetch(Mime::Url url) {

    if (url.scheme == "about") {
        if (url.path.str() == "./blank")
            return fetch("bundle://hideo-browser/blank.xhtml"_url);

        if (url.path.str() == "./start")
            return fetch("bundle://hideo-browser/start-page.xhtml"_url);

        return Error::invalidInput("unsupported about page");
    } // namespace Vaev

    auto start = Sys::now();

    auto mime = Mime::sniffSuffix(url.path.suffix());

    if (not mime.has())
        return Error::invalidInput("cannot determine MIME type");

    auto dom = makeStrong<Dom::Document>();
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    if (mime->is("text/html"_mime)) {
        Html::Parser parser{dom};
        parser.write(buf);

        auto elapsed = Sys::now() - start;
        logDebug("parse time: {}ms", elapsed.toUSecs() / 1000.0);

        return Ok(dom);
    } else if (mime->is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Xml::Parser parser;
        dom = try$(parser.parse(scan, HTML));

        auto elapsed = Sys::now() - start;
        logDebug("parse time: {}ms", elapsed.toUSecs() / 1000.0);

        return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);
        return Error::invalidInput("unsupported MIME type");
    }
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

    Vaev::Vec2Px viewport{Vaev::Px{800}, Vaev::Px{600}};
    auto media = Vaev::constructMedia(viewport);
    Vaev::View::render(*dom, media, viewport);

    co_return Ok();
}
