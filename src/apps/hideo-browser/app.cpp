#include <hideo-base/alert.h>
#include <hideo-base/scafold.h>
#include <karm-kira/context-menu.h>
#include <karm-kira/dialog.h>
#include <karm-kira/side-panel.h>
#include <karm-mime/mime.h>
#include <karm-sys/file.h>
#include <karm-sys/launch.h>
#include <karm-ui/input.h>
#include <karm-ui/popover.h>
#include <karm-ui/scroll.h>
#include <vaev-driver/fetcher.h>
#include <vaev-view/inspect.h>
#include <vaev-view/view.h>

namespace Hideo::Browser {

enum struct SidePanel {
    CLOSE,
    BOOKMARKS,
    DEVELOPER_TOOLS,
};

struct State {
    Mime::Url url;
    Res<Strong<Vaev::Dom::Document>> dom;
    SidePanel sidePanel = SidePanel::CLOSE;

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

using Action = Union<Reload, GoBack, GoForward, SidePanel>;

void reduce(State &s, Action a) {
    a.visit(Visitor{
        [&](Reload) {
            s.dom = Vaev::Driver::fetchDocument(s.url);
        },
        [&](GoBack) {
        },
        [&](GoForward) {
        },
        [&](SidePanel p) {
            s.sidePanel = p;
        },
    });
}

using Model = Ui::Model<State, Action, reduce>;

Ui::Child mainMenu([[maybe_unused]] State const &s) {
    return Kr::contextMenuContent({
        Kr::contextMenuItem(Model::bind(SidePanel::BOOKMARKS), Mdi::BOOKMARK, "Bookmarks"),
        Ui::separator(),
        Kr::contextMenuItem(Ui::NOP, Mdi::PRINTER, "Print..."),
#ifdef __ck_host__
        Kr::contextMenuItem(
            [&](auto &n) {
                auto res = Sys::launch(Mime::Uti::PUBLIC_OPEN, s.url);
                if (not res)
                    Ui::showDialog(
                        n,
                        Kr::alert(
                            "Error"s,
                            Io::format("Failed to open in default browser\n\n{}", res).unwrap()
                        )
                    );
            },
            Mdi::WEB, "Open in default browser..."
        ),
#endif
        Ui::separator(),
        Kr::contextMenuItem(Model::bind(SidePanel::DEVELOPER_TOOLS), Mdi::CODE_TAGS, "Developer Tools"),
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

Ui::Child contextMenu(State const &s) {
    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Kr::contextMenuIcon(Ui::NOP, Mdi::ARROW_LEFT),
            Kr::contextMenuIcon(Ui::NOP, Mdi::ARROW_RIGHT),
            Kr::contextMenuIcon(Ui::NOP, Mdi::REFRESH),
        }),
        Ui::separator(),
        Kr::contextMenuItem(
            [s](auto &) {
                (void)Sys::launch(Mime::Uti::PUBLIC_MODIFY, s.url);
            },
            Mdi::CODE_TAGS, "View Source..."
        ),
        Kr::contextMenuItem(Model::bind(SidePanel::DEVELOPER_TOOLS), Mdi::BUTTON_CURSOR, "Inspect"),
    });
}

Ui::Child inspectorContent(State const &s) {
    if (not s.dom) {
        return Ui::labelMedium(Ui::GRAY500, "No document") |
               Ui::center();
    }

    return Vaev::View::inspect(s.dom.unwrap()) | Ui::vhscroll();
}

Ui::Child sidePanel(State const &s) {
    switch (s.sidePanel) {
    case SidePanel::BOOKMARKS:
        return Kr::sidePanelContent({
            Kr::sidePanelTitle(Model::bind(SidePanel::CLOSE), "Bookmarks"),
            Ui::separator(),
            Ui::labelMedium(Ui::GRAY500, "No bookmarks") |
                Ui::center() |
                Ui::grow(),
        });

    case SidePanel::DEVELOPER_TOOLS:
        return Kr::sidePanelContent({
            Kr::sidePanelTitle(Model::bind(SidePanel::CLOSE), "Developer Tools"),
            Ui::separator(),
            inspectorContent(s) | Ui::grow(),
        });

    default:
        return Ui::empty();
    }
}

Ui::Child alert(State const &s, String title, String body) {
    return Ui::vflow(
               16,
               Math::Align::CENTER,
               Hideo::alert(title, body),
               Ui::hflow(
                   16,
                   Ui::button(Model::bindIf<GoBack>(s.canGoBack()), "Go Back"),
                   Ui::button(Model::bind<Reload>(), Ui::ButtonStyle::primary(), "Reload")
               )
           ) |
           Ui::center();
}

Ui::Child webview(State const &s) {
    if (not s.dom)
        return alert(s, "The page could not be loaded"s, Io::toStr(s.dom).unwrap());

    return Vaev::View::view(s.dom.unwrap()) |
           Ui::vscroll() |
           Ui::box({
               .backgroundPaint = Gfx::WHITE,
           }) |
           Kr::contextMenu(slot$(contextMenu(s)));
}

Ui::Child appContent(State const &s) {
    if (s.sidePanel == SidePanel::CLOSE)
        return webview(s);
    return Ui::hflow(
        webview(s) | Ui::grow(),
        Ui::separator(),
        sidePanel(s)
    );
}

Ui::Child app(Mime::Url url, Res<Strong<Vaev::Dom::Document>> dom) {
    return Ui::reducer<Model>(
        {
            url,
            dom,
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
                        [&](Ui::Node &n) {
                            Ui::showPopover(n, n.bound().bottomEnd(), mainMenu(s));
                        },
                        Ui::ButtonStyle::subtle(),
                        Mdi::DOTS_HORIZONTAL
                    )
                ),
                .body = slot$(appContent(s)),
            });
        }
    );
}

} // namespace Hideo::Browser
