#include <karm-app/form-factor.h>
#include <karm-kira/toolbar.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/popover.h>
#include <mdi/menu-open.h>
#include <mdi/menu.h>

#include "scaffold.h"

namespace Karm::Kira {

Ui::Child mobilescaffold(Scaffold::State const &s, Scaffold const &scaffold) {
    Ui::Children body;

    if (scaffold.midleTools)
        body.pushBack(toolbar(scaffold.midleTools().unwrap()));

    if (s.sidebarOpen and scaffold.sidebar) {
        body.pushBack(
            (scaffold.sidebar().unwrap()) |
            Ui::grow()
        );
    } else {
        body.pushBack(scaffold.body() | Ui::grow());
    }

    Ui::Children tools;

    if (scaffold.sidebar)
        tools.pushBack(
            Ui::button(
                Scaffold::Model::bind<Scaffold::ToggleSidebar>(),
                Ui::ButtonStyle::subtle(),
                s.sidebarOpen
                    ? Mdi::MENU_OPEN
                    : Mdi::MENU
            )
        );

    if (scaffold.startTools)
        tools.pushBack(
            hflow(4, scaffold.startTools().unwrap())
        );

    if (scaffold.startTools and scaffold.endTools)
        tools.pushBack(Ui::grow(NONE));

    if (scaffold.endTools)
        tools.pushBack(
            hflow(4, scaffold.endTools().unwrap())
        );

    if (tools.len())
        body.pushBack(bottombar(tools));

    return Ui::vflow(body) |
           Ui::pinSize(Math::Vec2i{411, 731}) |
           Ui::dialogLayer() |
           Ui::popoverLayer();
}

Ui::Child desktopscaffold(Scaffold::State const &s, Scaffold const &scaffold) {
    Ui::Children body;

    body.pushBack(titlebar(scaffold.icon, scaffold.title, scaffold.titlebar));

    Ui::Children tools;

    if (scaffold.sidebar)
        tools.pushBack(
            button(
                Scaffold::Model::bind<Scaffold::ToggleSidebar>(),
                Ui::ButtonStyle::subtle(),
                s.sidebarOpen ? Mdi::MENU_OPEN : Mdi::MENU
            )
        );

    if (scaffold.startTools)
        tools.pushBack(
            hflow(4, scaffold.startTools().unwrap())
        );

    if (scaffold.midleTools)
        tools.pushBack(
            hflow(4, scaffold.midleTools().unwrap()) | Ui::grow()
        );

    else if (scaffold.endTools)
        tools.pushBack(Ui::grow(NONE));

    if (scaffold.endTools)
        tools.pushBack(
            hflow(4, scaffold.endTools().unwrap())
        );

    if (tools.len())
        body.pushBack(
            toolbar(tools)
        );
    else
        body.pushBack(
            Ui::separator()
        );

    if (s.sidebarOpen and scaffold.sidebar) {
        body.pushBack(
            hflow(
                scaffold.sidebar().unwrap(),
                Ui::separator(),
                scaffold.body() | Ui::grow()
            ) |
            Ui::grow()
        );
    } else {
        body.pushBack(scaffold.body() | Ui::grow());
    }

    return Ui::vflow(body) |
           Ui::pinSize(scaffold.size) |
           Ui::dialogLayer() |
           Ui::popoverLayer();
}

Ui::Child scaffold(Scaffold scaffold) {
    auto isMobile = App::useFormFactor() == App::FormFactor::MOBILE;

    Scaffold::State state{
        .sidebarOpen = not isMobile,
        .isMobile = isMobile,
    };

    return Ui::reducer<Scaffold::Model>(state, [scaffold = std::move(scaffold)](Scaffold::State const &state) {
        return state.isMobile
                   ? mobilescaffold(state, scaffold)
                   : desktopscaffold(state, scaffold);
    });
}

} // namespace Karm::Kira
