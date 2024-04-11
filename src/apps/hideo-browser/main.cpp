#include <hideo-base/scafold.h>
#include <karm-mime/mime.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-ui/app.h>
#include <web-html/parser.h>
#include <web-view/view.h>
#include <web-xml/parser.h>

namespace Hideo::Browser {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::WEB,
        .title = "Browser"s,
        .startTools = slots$(
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::REFRESH),
        ),
        .body = [] {
            return Web::view();
        },
    });
}

} // namespace Hideo::Browser

Res<> entryPoint(Sys::Ctx &ctx) {
    auto args = Sys::useArgs(ctx);
    auto url = args.len()
                   ? try$(Mime::parseUrlOrPath(args[0]))
                   : "bundle://hideo-browser/start-page.html"_url;

    auto mime = Mime::sniffSuffix(url.path.suffix());

    if (not mime.has())
        return Error::invalidInput("cannot determine MIME type");

    logInfo("loading {}...", url);
    auto dom = makeStrong<Web::Dom::Document>();
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    if (mime->is("text/html"_mime)) {
        Web::Html::Parser parser{dom};
        parser.write(buf);
    } else if (mime->is("text/xml"_mime)) {
        Io::SScan scan{buf};
        Web::Xml::Parser parser;
        dom = try$(parser.parse(scan, Web::HTML));
    } else {
        logError("unsupported MIME type: {}", mime);
        return Error::invalidInput("unsupported MIME type");
    }

    return Ui::runApp(ctx, Hideo::Browser::app());
}
