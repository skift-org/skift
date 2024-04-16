#include <hideo-base/scafold.h>
#include <karm-mime/mime.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-ui/app.h>
#include <web-html/parser.h>
#include <web-view/inspect.h>
#include <web-view/view.h>
#include <web-xml/parser.h>

namespace Hideo::Browser {

Ui::Child app(Strong<Web::Dom::Document> dom) {
    return Hideo::scafold({
        .icon = Mdi::WEB,
        .title = "Browser"s,
        .startTools = slots$(
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::REFRESH),
        ),
        .body = [=] {
            return Ui::hflow(
                Web::View::view(dom) | Ui::grow(),
                Ui::separator(),
                Web::View::inspect(dom)
            );
        },
    });
}

} // namespace Hideo::Browser

Res<Strong<Web::Dom::Document>> fetch(Mime::Url url) {
    logInfo("fetching: {}", url);

    if (url.scheme == "about") {
        if (url.path.str() == "./blank")
            return Ok(makeStrong<Web::Dom::Document>());
        if (url.path.str() == "./start")
            return fetch("bundle://hideo-browser/start-page.html"_url);
        return Error::invalidInput("unsupported about page");
    }

    auto mime = Mime::sniffSuffix(url.path.suffix());

    if (not mime.has())
        return Error::invalidInput("cannot determine MIME type");

    auto dom = makeStrong<Web::Dom::Document>();
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    if (mime->is("text/html"_mime)) {
        Web::Html::Parser parser{dom};
        parser.write(buf);

        return Ok(dom);
    } else if (mime->is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Web::Xml::Parser parser;
        dom = try$(parser.parse(scan, Web::HTML));

        return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);
        return Error::invalidInput("unsupported MIME type");
    }
}

Res<> entryPoint(Sys::Ctx &ctx) {
    auto args = Sys::useArgs(ctx);
    auto url = args.len()
                   ? try$(Mime::parseUrlOrPath(args[0]))
                   : "about:start"_url;

    auto dom = try$(fetch(url));

    return Ui::runApp(
        ctx,
        Hideo::Browser::app(dom)
    );
}
