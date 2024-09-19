#pragma once

#include <karm-kira/side-panel.h>
#include <karm-ui/layout.h>
#include <mdi/dock-right.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_SIDE_PANEL{
    Mdi::DOCK_RIGHT,
    "Side Panel",
    "A panel that slides in from the side of the screen to display aditional information or properties",
    [] {
        return Ui::state(false, [](auto state, auto bind) {
            auto content = Ui::button(bind(!state), "Toggle Side Panel") |
                           Ui::center() |
                           Ui::grow();

            if (not state) {
                return content;
            }

            return Ui::hflow(
                content,
                Ui::separator(),
                Kr::sidePanelContent({
                    Kr::sidePanelTitle(bind(false), "Side Panel"),
                    Ui::separator(),
                    Ui::labelMedium(Ui::GRAY500, "This is a side panel.") |
                        Ui::center() | Ui::grow(),
                })
            );
        });
    },
};

} // namespace Hideo::Zoo
