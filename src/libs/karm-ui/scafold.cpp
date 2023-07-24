#include <karm-main/base.h>

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
                 button(bindBubble<Events::ExitEvent>(Ok()), ButtonStyle::subtle(), Mdi::CLOSE));
}

Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style) {
    auto isMobile = useFormFactor() == FormFactor::MOBILE;

    if (isMobile) {
        return empty();
    }

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
           dragRegion() | box(TOOLBAR);
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

Child scafold(Scafold scafold) {
    auto isMobile = useFormFactor() == FormFactor::MOBILE;

    Scafold::State state{
        .sidebarOpen = not isMobile,
        .isMobile = isMobile,
    };

    return reducer<Scafold::Model>(state, [scafold = std::move(scafold)](Scafold::State const &state) {
        Children appBody;
        appBody.pushBack(titlebar(scafold.icon, scafold.title, scafold.titlebar));

        if (state.isMobile) {
            if (scafold.midleTools.len())
                appBody.pushBack(
                    toolbar(scafold.midleTools));
        } else {
            Children tools;

            if (scafold.sidebar)
                tools.pushBack(
                    button(Scafold::Model::bind<Scafold::ToggleSidebar>(), Ui::ButtonStyle::subtle(), state.sidebarOpen ? Mdi::MENU_OPEN : Mdi::MENU));

            if (scafold.startTools.len())
                tools.pushBack(
                    hflow(4, scafold.startTools));

            if (scafold.midleTools.len())
                tools.pushBack(
                    hflow(4, scafold.midleTools) | grow());

            if (scafold.endTools.len())
                tools.pushBack(
                    hflow(4, scafold.endTools));

            if (tools.len())
                appBody.pushBack(
                    toolbar(tools));
            else
                appBody.pushBack(
                    separator());
        }

        if (state.isMobile) {
            if (state.sidebarOpen and scafold.sidebar) {
                appBody.pushBack((*scafold.sidebar) |
                                 Ui::box({.backgroundPaint = GRAY900}) |
                                 grow());
            } else {
                appBody.pushBack(scafold.body | grow());
            }
        } else {
            if (state.sidebarOpen and scafold.sidebar) {
                appBody.pushBack(hflow(
                                     *scafold.sidebar,
                                     separator(),
                                     scafold.body | grow()) |
                                 grow());
            } else {
                appBody.pushBack(scafold.body | grow());
            }
        }

        if (state.isMobile) {
            Children tools;

            if (scafold.sidebar)
                tools.pushBack(
                    button(
                        Scafold::Model::bind<Scafold::ToggleSidebar>(),
                        Ui::ButtonStyle::subtle(),
                        state.sidebarOpen
                            ? Mdi::MENU_OPEN
                            : Mdi::MENU));

            if (scafold.startTools.len())
                tools.pushBack(
                    hflow(4, scafold.startTools));

            if (scafold.startTools.len() and scafold.endTools.len())
                tools.pushBack(grow(NONE));

            if (scafold.endTools.len())
                tools.pushBack(
                    hflow(4, scafold.endTools));

            if (tools.len())
                appBody.pushBack(
                    bottombar(tools));
        }

        return vflow(appBody) |
               pinSize(state.isMobile ? Math::Vec2i{411, 731} : scafold.size) |
               dialogLayer();
    });
}

} // namespace Karm::Ui
