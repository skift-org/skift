#include <karm-app/host.h>
#include <karm-kira/toolbar.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/popover.h>
#include <karm-ui/view.h>

#include "scafold.h"

namespace Hideo {

Ui::Child mobileScafold(Scafold::State const &s, Scafold const &scafold) {
    Ui::Children body;

    if (scafold.midleTools)
        body.pushBack(Kr::toolbar(scafold.midleTools().unwrap()));

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
        body.pushBack(Kr::bottombar(tools));

    return Ui::vflow(body) |
           Ui::pinSize(Math::Vec2i{411, 731}) |
           Ui::dialogLayer() |
           Ui::popoverLayer();
}

Ui::Child desktopScafold(Scafold::State const &s, Scafold const &scafold) {
    Ui::Children body;

    body.pushBack(Kr::titlebar(scafold.icon, scafold.title, scafold.titlebar));

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
            Kr::toolbar(tools)
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
