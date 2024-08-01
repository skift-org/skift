#include <karm-ui/anim.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>

#include "side-nav.h"

namespace Karm::Kira {

Ui::Child sidenav(Ui::Children children) {
    return Ui::vflow(8, children) |
           Ui::insets(8) |
           Ui::vscroll() |
           Ui::minSize({198, Ui::UNCONSTRAINED});
}

Ui::Child sidenavTree(Mdi::Icon icon, String title, Ui::Slot child) {
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
                ) | Ui::insets({0, 8, 12, 8})
            ),

            state
                ? child() |
                      Ui::insets({32, 0, 0, 0}) |
                      Ui::slideIn(Ui::SlideFrom::TOP)
                : Ui::empty()
        );
    });
}

Ui::Child sidenavItem(bool selected, Ui::OnPress onPress, Mdi::Icon icon, String title) {
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
            {0, 8, 12, 8},
            hflow(
                indicator,
                Ui::empty(8),
                Ui::icon(icon, 18),
                Ui::empty(12),
                Ui::labelMedium(title) | Ui::center()
            )
        )
    );
}

} // namespace Karm::Kira
