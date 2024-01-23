#include <karm-sys/context.h>

#include "dialog.h"
#include "drag.h"
#include "input.h"
#include "layout.h"
#include "scafold.h"
#include "view.h"

namespace Karm::Ui {

static BoxStyle TOOLBAR = {
    .backgroundPaint = GRAY900,
};

Child aboutButton(Mdi::Icon icon, String title) {
    return button(
        rbind(showAboutDialog, icon, title),
        ButtonStyle::subtle(), icon, title);
}

Child controls(TitlebarStyle style) {
    return hflow(12,
                 button(bindBubble<Events::RequestMinimizeEvent>(), ButtonStyle::subtle(), Mdi::MINUS) |
                     cond(style == TitlebarStyle::DEFAULT),
                 button(bindBubble<Events::RequestMaximizeEvent>(), ButtonStyle::subtle(), Mdi::CROP_SQUARE) |
                     cond(style == TitlebarStyle::DEFAULT),
                 button(bindBubble<Events::RequestExitEvent>(), ButtonStyle::subtle(), Mdi::CLOSE));
}

Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style) {
    return hflow(
               4,
               aboutButton(icon, title),
               grow(NONE),
               controls(style)) |
           spacing(8) |
           dragRegion() | box(TOOLBAR);
}

Child titlebar(Mdi::Icon icon, String title, Child tabs, TitlebarStyle style) {
    return hflow(
               4,
               aboutButton(icon, title),
               tabs | Ui::grow(),
               controls(style)) |
           spacing(8) |
           dragRegion();
}

Child toolbar(Children children) {
    return vflow(
        hflow(4, children) |
            spacing(8) |
            box(TOOLBAR),
        separator());
}

Ui::Child bottombar(Children children) {
    return vflow(
        separator(),
        hflow(4, children) |
            spacing(8) |
            box(TOOLBAR));
}

Child mobileScafold(Scafold::State const &s, Scafold const &scafold) {
    Children body;

    if (scafold.midleTools)
        body.pushBack(toolbar(scafold.midleTools().unwrap()));

    if (s.sidebarOpen and scafold.sidebar) {
        body.pushBack((scafold.sidebar().unwrap()) |
                      Ui::box({.backgroundPaint = GRAY900}) |
                      grow());
    } else {
        body.pushBack(scafold.body() | grow());
    }

    Children tools;

    if (scafold.sidebar)
        tools.pushBack(
            button(
                Scafold::Model::bind<Scafold::ToggleSidebar>(),
                Ui::ButtonStyle::subtle(),
                s.sidebarOpen
                    ? Mdi::MENU_OPEN
                    : Mdi::MENU));

    if (scafold.startTools)
        tools.pushBack(
            hflow(4, scafold.startTools().unwrap()));

    if (scafold.startTools and scafold.endTools)
        tools.pushBack(grow(NONE));

    if (scafold.endTools)
        tools.pushBack(
            hflow(4, scafold.endTools().unwrap()));

    if (tools.len())
        body.pushBack(bottombar(tools));

    return vflow(body) |
           pinSize(Math::Vec2i{411, 731}) |
           dialogLayer();
}

Child desktopScafold(Scafold::State const &s, Scafold const &scafold) {
    Children body;

    body.pushBack(titlebar(scafold.icon, scafold.title, scafold.titlebar));

    Children tools;

    if (scafold.sidebar)
        tools.pushBack(
            button(Scafold::Model::bind<Scafold::ToggleSidebar>(), Ui::ButtonStyle::subtle(), s.sidebarOpen ? Mdi::MENU_OPEN : Mdi::MENU));

    if (scafold.startTools)
        tools.pushBack(
            hflow(4, scafold.startTools().unwrap()));

    if (scafold.midleTools)
        tools.pushBack(
            hflow(4, scafold.midleTools().unwrap()) | grow());

    else if (scafold.endTools)
        tools.pushBack(grow(NONE));

    if (scafold.endTools)
        tools.pushBack(
            hflow(4, scafold.endTools().unwrap()));

    if (tools.len())
        body.pushBack(
            toolbar(tools));
    else
        body.pushBack(
            separator());

    if (s.sidebarOpen and scafold.sidebar) {
        body.pushBack(
            hflow(
                scafold.sidebar().unwrap(),
                separator(),
                scafold.body() | grow()) |
            grow());
    } else {
        body.pushBack(scafold.body() | grow());
    }

    return vflow(body) |
           pinSize(scafold.size) |
           dialogLayer();
}

Child scafold(Scafold scafold) {
    auto isMobile = Sys::useFormFactor() == Sys::FormFactor::MOBILE;

    Scafold::State state{
        .sidebarOpen = not isMobile,
        .isMobile = isMobile,
    };

    return reducer<Scafold::Model>(state, [scafold = std::move(scafold)](Scafold::State const &state) {
        return state.isMobile
                   ? mobileScafold(state, scafold)
                   : desktopScafold(state, scafold);
    });
}

} // namespace Karm::Ui
