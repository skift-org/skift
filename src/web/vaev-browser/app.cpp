#include <karm-kira/context-menu.h>
#include <karm-kira/dialog.h>
#include <karm-kira/error-page.h>
#include <karm-kira/scaffold.h>
#include <karm-kira/side-panel.h>
#include <karm-mime/mime.h>
#include <karm-sys/file.h>
#include <karm-sys/launch.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/popover.h>
#include <karm-ui/resizable.h>
#include <karm-ui/scroll.h>
#include <mdi/alert-decagram.h>
#include <mdi/arrow-left.h>
#include <mdi/bookmark-outline.h>
#include <mdi/bookmark.h>
#include <mdi/button-cursor.h>
#include <mdi/close.h>
#include <mdi/code-tags.h>
#include <mdi/cog.h>
#include <mdi/dots-horizontal.h>
#include <mdi/google-downasaur.h>
#include <mdi/home.h>
#include <mdi/lock.h>
#include <mdi/printer.h>
#include <mdi/refresh.h>
#include <mdi/surfing.h>
#include <mdi/tune-variant.h>
#include <mdi/web.h>
#include <vaev-driver/fetcher.h>
#include <vaev-view/view.h>

#include "inspect.h"

namespace Vaev::Browser {

enum struct SidePanel {
    CLOSE,
    BOOKMARKS,
    DEVELOPER_TOOLS,
};

struct State {
    Mime::Url url;
    Res<Strong<Markup::Document>> dom;
    SidePanel sidePanel = SidePanel::CLOSE;
    InspectState inspect = {};

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

using Action = Union<Reload, GoBack, GoForward, SidePanel, InspectorAction>;

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
        [&](InspectorAction a) {
            s.inspect.apply(a);
        }
    });
}

using Model = Ui::Model<State, Action, reduce>;

Ui::Child mainMenu([[maybe_unused]] State const &s) {
    return Kr::contextMenuContent({
        Kr::contextMenuItem(
            Ui::NOP,
            Mdi::BOOKMARK_OUTLINE, "Add bookmark..."
        ),
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

Ui::Child addressMenu() {
    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Ui::labelMedium("Your are viewing a secure page") | Ui::insets(8),
            Kr::contextMenuIcon(Ui::NOP, Mdi::CLOSE),
        }),
    });
}

Ui::Child addressBar(Mime::Url const &url) {
    return Ui::hflow(
               Ui::button(
                   [&](auto &n) {
                       Ui::showPopover(n, n.bound().bottomStart(), addressMenu());
                   },
                   Ui::ButtonStyle::subtle(), Mdi::TUNE_VARIANT
               ),
               Ui::text("{}", url) |
                   Ui::vcenter() |
                   Ui::hscroll() |
                   Ui::grow(),
               Ui::button(Model::bind<Reload>(), Ui::ButtonStyle::subtle(), Mdi::REFRESH)
           ) |
           Ui::box({
               .padding = {0, 0, 0, 0},
               .borderRadii = 4,
               .borderWidth = 1,
               .backgroundFill = Ui::GRAY800,
           });
}

Ui::Child contextMenu(State const &s) {
    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Kr::contextMenuIcon(Ui::NOP, Mdi::ARROW_LEFT),
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

    return Vaev::Browser::inspect(
        s.dom.unwrap(),
        s.inspect,
        [&](auto &n, auto a) {
            Model::bubble(n, a);
        }
    );
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
    return Kr::errorPageContent({
        Kr::errorPageTitle(Mdi::GOOGLE_DOWNASAUR, title),
        Kr::errorPageBody(body),
        Kr::errorPageFooter({
            Ui::button(Model::bindIf<GoBack>(s.canGoBack()), "Go Back"),
            Ui::button(Model::bind<Reload>(), Ui::ButtonStyle::primary(), "Reload"),
        }),
    });
}

Ui::Child webview(State const &s) {
    if (not s.dom)
        return alert(s, "The page could not be loaded"s, Io::toStr(s.dom).unwrap());

    return Vaev::View::view(s.dom.unwrap()) |
           Ui::vscroll() |
           Ui::box({
               .backgroundFill = Gfx::WHITE,
           }) |
           Kr::contextMenu(slot$(contextMenu(s)));
}

Ui::Child appContent(State const &s) {
    if (s.sidePanel == SidePanel::CLOSE)
        return webview(s);
    return Ui::hflow(
        webview(s) | Ui::grow(),
        sidePanel(s) | Ui::resizable(Ui::ResizeHandle::START, {320}, NONE)
    );
}

Ui::Child app(Mime::Url url, Res<Strong<Vaev::Markup::Document>> dom) {
    return Ui::reducer<Model>(
        {
            url,
            dom,
        },
        [](State const &s) {
            return Kr::scaffold({
                .icon = Mdi::SURFING,
                .title = "Vaev"s,
                .startTools = slots$(
                    Ui::button(Model::bindIf<GoBack>(s.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
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
                .size = {1024, 720},
                .compact = true,
            });
        }
    );
}

} // namespace Vaev::Browser
