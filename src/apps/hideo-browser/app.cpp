#include <hideo-base/scafold.h>
#include <karm-kira/context-menu.h>
#include <karm-mime/mime.h>
#include <karm-sys/file.h>
#include <karm-ui/input.h>
#include <karm-ui/popover.h>
#include <web-html/parser.h>
#include <web-view/inspect.h>
#include <web-view/view.h>
#include <web-xml/parser.h>

namespace Hideo::Browser {

struct State {
    Mime::Url url;
    Strong<Web::Dom::Document> dom;
    Opt<Error> err;

    bool canGoBack() const {
        return false;
    }

    bool canGoForward() const {
        return false;
    }
};

struct Reload {};

struct GoBack {};

struct GoForward {};

using Action = Union<Reload, GoBack, GoForward>;

void reduce(State &, Action a) {
    a.visit(Visitor{
        [&](Reload) {
        },
        [&](GoBack) {
        },
        [&](GoForward) {
        },
    });
}

using Model = Ui::Model<State, Action, reduce>;

Ui::Child mainMenu() {
    return Kr::contextMenuContent({
        Kr::contextMenuItem(Ui::NOP, Mdi::BOOKMARK, "Bookmarks"),
        Ui::separator(),
        Kr::contextMenuItem(Ui::NOP, Mdi::PRINTER, "Print..."),
        Kr::contextMenuItem(Ui::NOP, Mdi::CODE_TAGS, "Developer Tools"),
        Ui::separator(),
        Kr::contextMenuItem(Ui::NOP, Mdi::COG, "Settings"),
    });
}

Ui::Child addressBar(Mime::Url const &url) {
    return Ui::hflow(
               0,
               Math::Align::CENTER,
               Ui::text("{}", url),
               Ui::grow(NONE),
               Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BOOKMARK_OUTLINE)
           ) |
           Ui::box({
               .padding = {12, 0, 0, 0},
               .borderRadius = 4,
               .borderWidth = 1,
               .backgroundPaint = Ui::GRAY800,
           });
}

Ui::Child app(Mime::Url url, Strong<Web::Dom::Document> dom, Opt<Error> err) {
    return Ui::reducer<Model>(
        {
            url,
            dom,
            err,
        },
        [](State const &s) {
            return Hideo::scafold({
                .icon = Mdi::WEB,
                .title = "Browser"s,
                .startTools = slots$(
                    Ui::button(Model::bindIf<GoBack>(s.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
                    Ui::button(Model::bindIf<GoForward>(s.canGoForward()), Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
                    Ui::button(Model::bind<Reload>(), Ui::ButtonStyle::subtle(), Mdi::REFRESH)
                ),
                .midleTools = slots$(addressBar(s.url) | Ui::grow()),
                .endTools = slots$(
                    Ui::button(
                        [](Ui::Node &n) {
                            Ui::showPopover(n, n.bound().bottomEnd(), mainMenu());
                        },
                        Ui::ButtonStyle::subtle(), Mdi::DOTS_HORIZONTAL
                    )
                ),
                .body = slot$(Web::View::view(s.dom)),
            });
        }
    );
}

Res<Strong<Web::Dom::Document>> fetch(Mime::Url url) {
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

} // namespace Hideo::Browser
