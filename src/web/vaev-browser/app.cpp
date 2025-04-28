module;

#include <karm-app/inputs.h>
#include <karm-gc/heap.h>
#include <karm-gc/root.h>
#include <karm-gfx/colors.h>
#include <karm-sys/async.h>
#include <karm-sys/file.h>
#include <karm-sys/launch.h>
#include <karm-sys/time.h>
#include <vaev-dom/document.h>

export module Vaev.Browser:app;

import Mdi;
import Vaev.View;
import Vaev.Driver;
import Karm.Http;
import Karm.Kira;
import Karm.Ui;
import :inspect;

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

enum struct Status {
    LOADING,
    LOADED,
};

struct State {
    Gc::Heap& heap;
    Http::Client& client;
    Status status = Status::LOADED;
    usize currentIndex = 0;
    Vec<Navigate> history;
    Res<Gc::Root<Dom::Document>> dom;
    SidePanel sidePanel = SidePanel::CLOSE;
    InspectState inspect = {};
    String location;
    bool wireframe = false;

    State(Gc::Heap& heap, Http::Client& client, Navigate nav, Res<Gc::Root<Dom::Document>> dom)
        : heap{heap},
          client{client},
          history{nav},
          dom{dom}, location(dom ? dom.unwrap()->url().str() : String{}) {}

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

struct Loaded {
    Res<Gc::Root<Dom::Document>> dom;
};

struct GoBack {};

struct GoForward {};

struct ToggleWireframe {};

struct UpdateLocation {
    String location;
};

struct NavigateLocation {
};

using Action = Union<
    Reload,
    Loaded,
    GoBack,
    GoForward,
    ToggleWireframe,
    SidePanel,
    InspectorAction,
    Navigate,
    UpdateLocation,
    NavigateLocation>;

Async::_Task<Opt<Action>> navigateAsync(Gc::Heap& heap, Http::Client& client, Navigate nav) {
    if (nav.action == Mime::Uti::PUBLIC_MODIFY) {
        co_return Loaded{co_await Vaev::Driver::viewSourceAsync(heap, client, nav.url)};
    } else {
        co_return Loaded{co_await Vaev::Driver::fetchDocumentAsync(heap, client, nav.url)};
    }
}

Ui::Task<Action> reduce(State& s, Action a) {
    return a.visit(Visitor{
        [&](Reload) -> Ui::Task<Action> {
            if (s.status == Status::LOADING)
                return NONE;
            s.status = Status::LOADING;
            s.location = s.currentUrl().url.str();
            return navigateAsync(s.heap, s.client, s.currentUrl());
        },
        [&](Loaded l) -> Ui::Task<Action> {
            s.status = Status::LOADED;
            s.dom = l.dom;
            return NONE;
        },
        [&](GoBack) -> Ui::Task<Action> {
            s.currentIndex--;
            return reduce(s, Reload{});
        },
        [&](GoForward) -> Ui::Task<Action> {
            s.currentIndex++;
            return reduce(s, Reload{});
        },
        [&](ToggleWireframe) -> Ui::Task<Action> {
            s.wireframe = not s.wireframe;
            return NONE;
        },
        [&](SidePanel p) -> Ui::Task<Action> {
            if (s.sidePanel == p) {
                s.sidePanel = SidePanel::CLOSE;
            } else {
                s.sidePanel = p;
            }
            return NONE;
        },
        [&](InspectorAction a) -> Ui::Task<Action> {
            s.inspect.apply(a);
            return NONE;
        },
        [&](Navigate n) -> Ui::Task<Action> {
            s.history.trunc(s.currentIndex + 1);
            s.history.pushBack(n);
            s.currentIndex++;
            return reduce(s, Reload{});
        },
        [&](UpdateLocation u) -> Ui::Task<Action> {
            s.location = u.location;
            return NONE;
        },
        [&](NavigateLocation) -> Ui::Task<Action> {
            return reduce(s, Navigate{Mime::Url::parse(s.location)});
        },
    });

    return NONE;
}

using Model = Ui::Model<State, Action, reduce>;

#ifdef __ck_host__

Ui::Child openInDefaultBrowser(State const& s) {
    return Kr::contextMenuItem(
        [&](auto& n) {
            auto url = s.currentUrl().url;
            if (url.scheme != "http" and url.scheme != "https" and url.scheme != "file") {
                Ui::showDialog(
                    n,
                    Kr::alertDialog(
                        "Could not open in default browser"s,
                        Io::format("Only http, https, and file urls can be opened in the default browser.")
                    )
                );
                return;
            }

            auto res = Sys::launch({
                .action = Mime::Uti::PUBLIC_OPEN,
                .objects = {s.currentUrl().url},
            });

            if (not res)
                Ui::showDialog(
                    n,
                    Kr::alertDialog(
                        "Could not open in default browser"s,
                        Io::format("Failed to open in default browser\n\n{}", res)
                    )
                );
        },
        Mdi::WEB, "Open in default browser..."
    );
}

#endif

Ui::Child mainMenu([[maybe_unused]] State const& s) {
    return Kr::contextMenuContent({
        Kr::contextMenuItem(
            Ui::SINK<>,
            Mdi::BOOKMARK_OUTLINE, "Add bookmark..."
        ),
        Kr::contextMenuItem(Model::bind(SidePanel::BOOKMARKS), Mdi::BOOKMARK, "Bookmarks"),
        Kr::separator(),
        Kr::contextMenuItem(
            not s.dom
                ? Opt<Ui::Send<>>{NONE}
                : [dom = s.dom.unwrap()](auto& n) {
                      Ui::showDialog(
                          n,
                          View::printDialog(dom)
                      );
                  },
            Mdi::PRINTER, "Print..."
        ),
#ifdef __ck_host__
        openInDefaultBrowser(s),
#endif
        Kr::separator(),
        Kr::contextMenuItem(Model::bind(SidePanel::DEVELOPER_TOOLS), Mdi::CODE_TAGS, "Developer Tools"),
        Kr::contextMenuCheck(Model::bind<ToggleWireframe>(), s.wireframe, "Show wireframe"),
        Kr::separator(),
        Kr::contextMenuItem(Ui::SINK<>, Mdi::COG, "Settings"),
    });
}

Ui::Child addressMenu() {
    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Ui::labelMedium("Your are viewing a secure page") | Ui::insets(8),
            Kr::contextMenuIcon(Ui::SINK<>, Mdi::CLOSE),
        }),
    });
}

Ui::Child reloadButton(State const& s) {
    return (
               s.status == Status::LOADING
                   ? Kr::progress()
                   : Ui::icon(Mdi::REFRESH)
           ) |
           Ui::insets(6) |
           Ui::center() |
           Ui::minSize({36, 36}) |
           Ui::button(Model::bind<Reload>(), Ui::ButtonStyle::subtle());
}

Ui::Child addressBar(State const& s) {
    return Ui::hflow(
               Ui::button(
                   [&](auto& n) {
                       Ui::showPopover(n, n.bound().bottomStart(), addressMenu());
                   },
                   Ui::ButtonStyle::subtle(), Mdi::TUNE_VARIANT
               ),
               Ui::input(Ui::TextStyles::labelMedium(), s.location, Model::map<UpdateLocation>()) |
                   Ui::vcenter() |
                   Ui::hscroll() |
                   Ui::grow()
           ) |
           Ui::box({
               .padding = {0, 0, 0, 0},
               .borderRadii = 4,
               .borderWidth = 1,
               .backgroundFill = Ui::GRAY800,
           }) |
           Ui::keyboardShortcut(App::Key::ENTER, Model::bind<NavigateLocation>()) |
           Ui::focusable() |
           Ui::keyboardShortcut(App::Key::L, App::KeyMod::CTRL);
}

Ui::Child contextMenu(State const& s) {
    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Kr::contextMenuIcon(Model::bindIf<GoBack>(s.canGoBack()), Mdi::ARROW_LEFT),
            Kr::contextMenuIcon(Model::bindIf<GoForward>(s.canGoForward()), Mdi::ARROW_RIGHT),
            Kr::contextMenuIcon(Model::bind<Reload>(), Mdi::REFRESH),
        }),
        Kr::separator(),
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
            Kr::separator(),
            Ui::labelMedium(Ui::GRAY500, "No bookmarks") |
                Ui::center() |
                Ui::grow(),
        });

    case SidePanel::DEVELOPER_TOOLS:
        return Kr::sidePanelContent({
            Kr::sidePanelTitle(Model::bind(SidePanel::CLOSE), "Developer Tools"),
            Kr::separator(),
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

    return Vaev::View::view(s.dom.unwrap(), {.wireframe = s.wireframe, .selected = s.inspect.selectedNode}) |
           Ui::vscroll() |
           Ui::box({
               .backgroundFill = Gfx::WHITE,
           }) |
           Kr::contextMenu([&] {
               return contextMenu(s);
           });
}

Ui::Child appContent(State const& s) {
    if (s.sidePanel == SidePanel::CLOSE) {
        return webview(s);
    }
    return Ui::hflow(
        webview(s) | Ui::grow(),
        sidePanel(s) | Kr::resizable(Kr::ResizeHandle::START, {320}, Ui::SINK<Math::Vec2i>)
    );
}

export Ui::Child app(Gc::Heap& heap, Http::Client& client, Mime::Url url, Res<Gc::Ref<Vaev::Dom::Document>> dom) {
    return Ui::reducer<Model>(
        {
            heap,
            client,
            Navigate{url},
            dom,
        },
        [](State const& s) {
            auto scaffold = Kr::scaffold({
                .icon = Mdi::SURFING,
                .title = "Vaev"s,
                .startTools = [&] -> Ui::Children {
                    return {
                        Ui::button(Model::bindIf<GoBack>(s.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT) | Ui::keyboardShortcut(App::Key::LEFT, App::KeyMod::ALT, Model::bind<GoBack>()) |
                            Ui::keyboardShortcut(App::Key::LEFT, App::KeyMod::ALT),
                        Ui::button(Model::bindIf<GoForward>(s.canGoForward()), Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT) |
                            Ui::keyboardShortcut(App::Key::RIGHT, App::KeyMod::ALT),
                        reloadButton(s),
                    };
                },
                .middleTools = [&] -> Ui::Children {
                    return {addressBar(s) | Ui::grow()};
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
            return scaffold |
                   Ui::keyboardShortcut(App::Key::R, App::KeyMod::CTRL, Model::bind<Reload>()) |
                   Ui::keyboardShortcut(App::Key::F5, Model::bind<Reload>()) |
                   Ui::keyboardShortcut(App::Key::F12, Model::bind(SidePanel::DEVELOPER_TOOLS)) |
                   Ui::keyboardShortcut(App::Key::B, App::KeyMod::CTRL, Model::bind(SidePanel::BOOKMARKS)) |
                   Ui::keyboardShortcut(App::Key::P, App::KeyMod::CTRL, [&](auto& n) {
                       if (not s.dom)
                           return;

                       Ui::showDialog(
                           n,
                           View::printDialog(s.dom.unwrap())
                       );
                   });
        }
    );
}

} // namespace Vaev::Browser
