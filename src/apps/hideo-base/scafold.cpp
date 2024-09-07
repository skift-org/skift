#include <karm-app/host.h>
#include <karm-kira/about-dialog.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/popover.h>
#include <karm-ui/view.h>

#include "scafold.h"

namespace Hideo {

#undef __ck_sys_darwin__

static Ui::BoxStyle TOOLBAR = {
    .backgroundFill = Ui::GRAY900,
};

Ui::Child aboutButton([[maybe_unused]] Mdi::Icon icon, String title) {
    return Ui::button(
        [title](auto &n) {
            Ui::showDialog(n, Kr::aboutDialog(title));
        },
        Ui::ButtonStyle::subtle(), icon, title
    );
}

Ui::Child controls(TitlebarStyle style) {
    return Ui::hflow(
        4,
        Ui::button(
            Ui::bindBubble<App::RequestMinimizeEvent>(),
            Ui::ButtonStyle::subtle(),
            Mdi::MINUS
        ) | Ui::cond(style == TitlebarStyle::DEFAULT),
        Ui::button(
            Ui::bindBubble<App::RequestMaximizeEvent>(),
            Ui::ButtonStyle::subtle(),
            Mdi::CROP_SQUARE
        ) | Ui::cond(style == TitlebarStyle::DEFAULT),
        Ui::button(
            Ui::bindBubble<App::RequestExitEvent>(),
            Ui::ButtonStyle::subtle(),
            Mdi::CLOSE
        )
    );
}

Ui::Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style) {
    return Ui::hflow(
               4,
               aboutButton(icon, title),
               Ui::grow(NONE),
               controls(style)
           ) |
           Ui::insets(8) |
           Ui::dragRegion() | box(TOOLBAR);
}

Ui::Child titlebar(Mdi::Icon icon, String title, Ui::Child tabs, TitlebarStyle style) {
    return hflow(
               4,
               aboutButton(icon, title),
               tabs | Ui::grow(),
               controls(style)
           ) |
           Ui::insets(8) |
           Ui::dragRegion();
}

Ui::Child toolbar(Ui::Children children) {
    return Ui::vflow(
        Ui::hflow(4, children) |
            Ui::insets(8) |
            box(TOOLBAR),
        Ui::separator()
    );
}

Ui::Child bottombar(Ui::Children children) {
    return Ui::vflow(
        Ui::separator(),
        Ui::hflow(4, children) |
            Ui::insets(8) |
            Ui::box(TOOLBAR)
    );
}

Ui::Child mobileScafold(Scafold::State const &s, Scafold const &scafold) {
    Ui::Children body;

    if (scafold.midleTools)
        body.pushBack(toolbar(scafold.midleTools().unwrap()));

    if (s.sidebarOpen and scafold.sidebar) {
        body.pushBack(
            (scafold.sidebar().unwrap()) |
            Ui::grow()
        );
    } else {
        body.pushBack(scafold.body() | Ui::grow());
    }

    Ui::Children tools;

    if (scafold.sidebar)
        tools.pushBack(
            Ui::button(
                Scafold::Model::bind<Scafold::ToggleSidebar>(),
                Ui::ButtonStyle::subtle(),
                s.sidebarOpen
                    ? Mdi::MENU_OPEN
                    : Mdi::MENU
            )
        );

    if (scafold.startTools)
        tools.pushBack(
            hflow(4, scafold.startTools().unwrap())
        );

    if (scafold.startTools and scafold.endTools)
        tools.pushBack(Ui::grow(NONE));

    if (scafold.endTools)
        tools.pushBack(
            hflow(4, scafold.endTools().unwrap())
        );

    if (tools.len())
        body.pushBack(bottombar(tools));

    return Ui::vflow(body) |
           Ui::pinSize(Math::Vec2i{411, 731}) |
           Ui::dialogLayer() |
           Ui::popoverLayer();
}

Ui::Child desktopScafold(Scafold::State const &s, Scafold const &scafold) {
    Ui::Children body;

    body.pushBack(titlebar(scafold.icon, scafold.title, scafold.titlebar));

    Ui::Children tools;

    if (scafold.sidebar)
        tools.pushBack(
            button(
                Scafold::Model::bind<Scafold::ToggleSidebar>(),
                Ui::ButtonStyle::subtle(),
                s.sidebarOpen ? Mdi::MENU_OPEN : Mdi::MENU
            )
        );

    if (scafold.startTools)
        tools.pushBack(
            hflow(4, scafold.startTools().unwrap())
        );

    if (scafold.midleTools)
        tools.pushBack(
            hflow(4, scafold.midleTools().unwrap()) | Ui::grow()
        );

    else if (scafold.endTools)
        tools.pushBack(Ui::grow(NONE));

    if (scafold.endTools)
        tools.pushBack(
            hflow(4, scafold.endTools().unwrap())
        );

    if (tools.len())
        body.pushBack(
            toolbar(tools)
        );
    else
        body.pushBack(
            Ui::separator()
        );

    if (s.sidebarOpen and scafold.sidebar) {
        body.pushBack(
            hflow(
                scafold.sidebar().unwrap(),
                Ui::separator(),
                scafold.body() | Ui::grow()
            ) |
            Ui::grow()
        );
    } else {
        body.pushBack(scafold.body() | Ui::grow());
    }

    return Ui::vflow(body) |
           Ui::pinSize(scafold.size) |
           Ui::dialogLayer() |
           Ui::popoverLayer();
}

Ui::Child scafold(Scafold scafold) {
    auto isMobile = Sys::useFormFactor() == Sys::FormFactor::MOBILE;

    Scafold::State state{
        .sidebarOpen = not isMobile,
        .isMobile = isMobile,
    };

    return Ui::reducer<Scafold::Model>(state, [scafold = std::move(scafold)](Scafold::State const &state) {
        return state.isMobile
                   ? mobileScafold(state, scafold)
                   : desktopScafold(state, scafold);
    });
}

} // namespace Hideo
