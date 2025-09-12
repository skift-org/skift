module;

#include <karm-gfx/icon.h>

export module Karm.Kira:sideNav;

import Karm.Core;
import Karm.Ui;
import Mdi;

namespace Karm::Kira {

export Ui::Child sidenav(Ui::Children children) {
    return Ui::vflow(8, children) |
           Ui::insets(8) |
           Ui::vscroll() |
           Ui::minSize({198, Ui::UNCONSTRAINED});
}

export Ui::Child sidenavTree(Gfx::Icon icon, String title, Ui::Slot child) {
    return Ui::state(true, [=, child = std::move(child)](bool state, auto bind) {
        return Ui::vflow(
            Ui::button(
                bind(not state),
                Ui::ButtonStyle::subtle(),
                Ui::hflow(
                    Ui::empty(8),
                    Ui::icon(icon, 18),
                    Ui::empty(12),
                    Ui::labelMedium(title) |
                        Ui::vcenter() |
                        Ui::grow(),
                    Ui::icon(state ? Mdi::CHEVRON_UP : Mdi::CHEVRON_DOWN, 18)
                ) | Ui::insets({8, 12, 8, 0})
            ),

            state
                ? child() |
                      Ui::insets({0, 0, 0, 32}) |
                      Ui::slideIn(Ui::SlideFrom::TOP)
                : Ui::empty()
        );
    });
}

export Ui::Child sidenavItem(bool selected, Opt<Ui::Send<>> onPress, Ui::Child content) {
    auto buttonStyle = Ui::ButtonStyle::regular();

    buttonStyle.idleStyle = {
        .borderRadii = 4,
        .backgroundFill = selected ? Ui::GRAY900 : Gfx::ALPHA,
    };

    auto indicator = box(
        {
            .borderRadii = 99,
            .backgroundFill = selected ? Ui::ACCENT600 : Gfx::ALPHA,
        },
        Ui::empty(2)
    );

    return Ui::button(
        std::move(onPress),
        buttonStyle,
        Ui::insets(
            {8, 12, 8, 0},
            hflow(
                indicator,
                Ui::empty(8),
                content
            )
        )
    );
}

export Ui::Child sidenavItem(bool selected, Opt<Ui::Send<>> onPress, Gfx::Icon icon, String title) {
    return sidenavItem(
        selected,
        onPress,
        Ui::hflow(
            Ui::icon(icon, 18),
            Ui::empty(12),
            Ui::labelMedium(title) | Ui::center()
        )
    );
}

export Ui::Child sidenavTitle(String title) {
    return Ui::titleMedium(title) | Ui::insets({8, 12, 8, 8});
}

} // namespace Karm::Kira
