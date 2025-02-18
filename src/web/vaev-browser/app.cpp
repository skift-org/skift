#include <karm-gc/root.h>
#include <karm-kira/context-menu.h>
#include <karm-kira/dialog.h>
#include <karm-kira/error-page.h>
#include <karm-kira/print-dialog.h>
#include <karm-kira/resizable.h>
#include <karm-kira/scaffold.h>
#include <karm-kira/side-panel.h>
#include <karm-mime/mime.h>
#include <karm-sys/file.h>
#include <karm-sys/launch.h>
#include <karm-ui/dialog.h>
#include <karm-ui/focus.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/popover.h>
#include <karm-ui/scroll.h>
#include <mdi/alert-decagram.h>
#include <mdi/arrow-left.h>
#include <mdi/arrow-right.h>
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
#include <vaev-view/dialog.h>
#include <vaev-view/view.h>

#include "inspect.h"

namespace Vaev::Browser {

enum struct SidePanel {
    CLOSE,
    BOOKMARKS,
    DEVELOPER_TOOLS,
};

struct Navigate {
    Mime::Url url;
    Mime::Uti action = Mime::Uti::PUBLIC_OPEN;
};

struct State {
    Gc::Heap& heap;
    usize currentIndex = 0;
    Vec<Navigate> history;
    Res<Gc::Root<Dom::Document>> dom;
    SidePanel sidePanel = SidePanel::CLOSE;
    InspectState inspect = {};
    bool wireframe = false;

    State(Gc::Heap& heap, Navigate nav, Res<Gc::Root<Dom::Document>> dom)
        : heap(heap), history{nav}, dom{dom} {}

    bool canGoBack() const {
        return currentIndex > 0;
    }

    bool canGoForward() const {
        return currentIndex < history.len() - 1;
    }

    Navigate const& currentUrl() const {
        return history[currentIndex];
    }
};

struct Reload {};

struct GoBack {};

struct GoForward {};

struct ToggleWireframe {};

using Action = Union<
    Reload,
    GoBack,
    GoForward,
    ToggleWireframe,
    SidePanel,
    InspectorAction,
    Navigate>;

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(Visitor{
        [&](Reload) {
            auto const& object = s.currentUrl();
            if (object.action == Mime::Uti::PUBLIC_MODIFY) {
                s.dom = Vaev::Driver::viewSource(s.heap, object.url);
            } else {
                s.dom = Vaev::Driver::fetchDocument(s.heap, object.url);
            }
        },
        [&](GoBack) {
            s.currentIndex--;
            reduce(s, Reload{}).unwrap();
        },
        [&](GoForward) {
            s.currentIndex++;
            reduce(s, Reload{}).unwrap();
        },
        [&](ToggleWireframe) {
            s.wireframe = not s.wireframe;
        },
        [&](SidePanel p) {
            s.sidePanel = p;
        },
        [&](InspectorAction a) {
            s.inspect.apply(a);
        },
        [&](Navigate n) {
            s.history.pushBack(n);
            s.currentIndex++;
            reduce(s, Reload{}).unwrap();
        },
    });

    return NONE;
}

using Model = Ui::Model<State, Action, reduce>;

Ui::Child mainMenu([[maybe_unused]] State const& s) {
    return Kr::contextMenuContent({
        Kr::contextMenuItem(
            Ui::NOP,
            Mdi::BOOKMARK_OUTLINE, "Add bookmark..."
        ),
        Kr::contextMenuItem(Model::bind(SidePanel::BOOKMARKS), Mdi::BOOKMARK, "Bookmarks"),
        Ui::separator(),
        Kr::contextMenuItem(
            not s.dom
                ? Ui::OnPress{NONE}
                : [dom = s.dom.unwrap()](auto& n) {
                      Ui::showDialog(
                          n,
                          View::printDialog(dom)
                      );
                  },
            Mdi::PRINTER, "Print..."
        ),
#ifdef __ck_host__
        Kr::contextMenuItem([&](auto& n) {
            auto res = Sys::launch({
                .action = Mime::Uti::PUBLIC_OPEN,
                .objects = {s.currentUrl().url},
            });

            if (not res)
                Ui::showDialog(
                    n,
                    Kr::alert(
                        "Error"s,
                        Io::format("Failed to open in default browser\n\n{}", res)
                    )
                );
        },
                            Mdi::WEB, "Open in default browser..."),
#endif
        Ui::separator(),
        Kr::contextMenuItem(Model::bind(SidePanel::DEVELOPER_TOOLS), Mdi::CODE_TAGS, "Developer Tools"),
        Kr::contextMenuCheck(Model::bind<ToggleWireframe>(), s.wireframe, "Show wireframe"),
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

Ui::Child addressBar(Mime::Url const& url) {
    return Ui::hflow(
               Ui::button(
                   [&](auto& n) {
                       Ui::showPopover(n, n.bound().bottomStart(), addressMenu());
                   },
                   Ui::ButtonStyle::subtle(), Mdi::TUNE_VARIANT
               ),
               Ui::input(Ui::TextStyles::labelMedium(), url.str(), NONE) |
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
           }) |
           Ui::focusable();
}

Ui::Child contextMenu(State const& s) {
    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Kr::contextMenuIcon(Model::bindIf<GoBack>(s.canGoBack()), Mdi::ARROW_LEFT),
            Kr::contextMenuIcon(Model::bindIf<GoForward>(s.canGoForward()), Mdi::ARROW_RIGHT),
            Kr::contextMenuIcon(Model::bind<Reload>(), Mdi::REFRESH),
        }),
        Ui::separator(),
        Kr::contextMenuItem(
            Model::bind<Navigate>(
                s.currentUrl().url,
                Mime::Uti::PUBLIC_MODIFY
            ),
            Mdi::CODE_TAGS, "View Source..."
        ),
        Kr::contextMenuItem(
            Model::bind(SidePanel::DEVELOPER_TOOLS),
            Mdi::BUTTON_CURSOR, "Inspect"
        ),
    });
}

Ui::Child inspectorContent(State const& s) {
    if (not s.dom) {
        return Ui::labelMedium(Ui::GRAY500, "No document") |
               Ui::center();
    }

    return Vaev::Browser::inspect(
        s.dom.unwrap(),
        s.inspect,
        [&](auto& n, auto a) {
            Model::bubble(n, a);
        }
    );
}

Ui::Child sidePanel(State const& s) {
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

Ui::Child alert(State const& s, String title, String body) {
    return Kr::errorPageContent({
        Kr::errorPageTitle(Mdi::GOOGLE_DOWNASAUR, title),
        Kr::errorPageBody(body),
        Kr::errorPageFooter({
            Ui::button(Model::bindIf<GoBack>(s.canGoBack()), "Go Back"),
            Ui::button(Model::bind<Reload>(), Ui::ButtonStyle::primary(), "Reload"),
        }),
    });
}

Ui::Child webview(State const& s) {
    if (not s.dom)
        return alert(s, "The page could not be loaded"s, Io::toStr(s.dom));

    return Vaev::View::view(s.dom.unwrap(), {.wireframe = s.wireframe}) |
           Ui::vscroll() |
           Ui::box({
               .backgroundFill = Gfx::WHITE,
           }) |
           Kr::contextMenu([&] {
               return contextMenu(s);
           });
}

Ui::Child appContent(State const& s) {
    if (s.sidePanel == SidePanel::CLOSE)
        return webview(s);
    return Ui::hflow(
        webview(s) | Ui::grow(),
        sidePanel(s) | Kr::resizable(Kr::ResizeHandle::START, {320}, NONE)
    );
}

Ui::Child app(Gc::Heap& heap, Mime::Url url, Res<Gc::Ref<Vaev::Dom::Document>> dom) {
    return Ui::reducer<Model>(
        {
            heap,
            Navigate{url},
            dom,
        },
        [](State const& s) {
            return Kr::scaffold({
                .icon = Mdi::SURFING,
                .title = "Vaev"s,
                .startTools = [&] -> Ui::Children {
                    return {
                        Ui::button(Model::bindIf<GoBack>(s.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
                        Ui::button(Model::bindIf<GoForward>(s.canGoForward()), Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
                    };
                },
                .middleTools = [&] -> Ui::Children {
                    return {addressBar(s.currentUrl().url) | Ui::grow()};
                },
                .endTools = [&] -> Ui::Children {
                    return {
                        Ui::button(
                            [&](Ui::Node& n) {
                                Ui::showPopover(n, n.bound().bottomEnd(), mainMenu(s));
                            },
                            Ui::ButtonStyle::subtle(),
                            Mdi::DOTS_HORIZONTAL
                        ),
                    };
                },
                .body = [&] {
                    return appContent(s);
                },
                .compact = true,
            });
        }
    );
}

} // namespace Vaev::Browser
