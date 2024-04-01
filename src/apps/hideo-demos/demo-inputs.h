#pragma once

#include <hideo-base/dialogs.h>
#include <hideo-base/row.h>
#include <hideo-base/scafold.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

static void willShowMessage(Ui::Node &n) {
    showMsgDialog(n, "Button pressed");
}

static inline Demo INPUTS_DEMO{
    Mdi::LIST_BOX_OUTLINE,
    "Inputs",
    "Form inputs widgets",
    [] {
        auto button = Hideo::buttonRow(
            willShowMessage,
            "Cool duck app", "Install"
        );

        auto title = Hideo::titleRow("Some Settings");

        auto list = Hideo::card(
            button,
            Ui::separator(),
            Hideo::treeRow(
                slot$(Ui::icon(Mdi::TOGGLE_SWITCH)), "Switches", NONE,
                slots$(
                    Hideo::toggleRow(true, NONE, "Some property"),
                    Hideo::toggleRow(true, NONE, "Some property"),
                    Hideo::toggleRow(true, NONE, "Some property")
                )
            ),

            Ui::separator(),
            Hideo::treeRow(slot$(Ui::icon(Mdi::CHECKBOX_MARKED)), "Checkboxs", NONE, slots$(Hideo::checkboxRow(true, NONE, "Some property"), Hideo::checkboxRow(false, NONE, "Some property"), Hideo::checkboxRow(false, NONE, "Some property"))),

            Ui::separator(),
            Hideo::treeRow(slot$(Ui::icon(Mdi::RADIOBOX_MARKED)), "Radios", NONE, slots$(Hideo::radioRow(true, NONE, "Some property"), Hideo::radioRow(false, NONE, "Some property"), Hideo::radioRow(false, NONE, "Some property"))),
            Ui::separator(),
            Hideo::sliderRow(0.5, NONE, "Some property")
        );

        return Ui::vflow(8, title, list) |
               Ui::maxSize({420, Ui::UNCONSTRAINED}) |
               Ui::grow() |
               Ui::hcenter() |
               Ui::vscroll();
    },
};

} // namespace Hideo::Demos
