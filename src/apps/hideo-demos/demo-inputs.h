#pragma once

#include <hideo-base/row.h>
#include <hideo-base/scafold.h>
#include <karm-kira/card.h>
#include <karm-kira/dialog.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

static void willShowMessage(Ui::Node &n) {
    Ui::showDialog(n, Kr::alert("Message"s, "This is a message"s));
}

static inline Demo INPUTS_DEMO{
    Mdi::LIST_BOX_OUTLINE,
    "Inputs",
    "Form inputs widgets",
    [] {
        auto button = Hideo::buttonRow(
            willShowMessage,
            "Cool duck app"s,
            "Install"s
        );

        auto title = Hideo::titleRow("Some Settings"s);

        auto list = Kr::card(
            button,
            Ui::separator(),
            Hideo::treeRow(
                slot$(Ui::icon(Mdi::TOGGLE_SWITCH)), "Switches"s, NONE,
                slots$(
                    Hideo::toggleRow(true, NONE, "Some property"s),
                    Hideo::toggleRow(true, NONE, "Some property"s),
                    Hideo::toggleRow(true, NONE, "Some property"s)
                )
            ),

            Ui::separator(),
            Hideo::treeRow(
                slot$(Ui::icon(Mdi::CHECKBOX_MARKED)),
                "Checkboxs"s,
                NONE,
                slots$(
                    Hideo::checkboxRow(true, NONE, "Some property"s),
                    Hideo::checkboxRow(false, NONE, "Some property"s),
                    Hideo::checkboxRow(false, NONE, "Some property"s)
                )
            ),

            Ui::separator(),
            Hideo::treeRow(
                slot$(Ui::icon(Mdi::RADIOBOX_MARKED)),
                "Radios"s,
                NONE,
                slots$(
                    Hideo::radioRow(true, NONE, "Some property"s),
                    Hideo::radioRow(false, NONE, "Some property"s),
                    Hideo::radioRow(false, NONE, "Some property"s)
                )
            ),
            Ui::separator(),
            Hideo::sliderRow(
                0.5,
                NONE,
                "Some property"s
            )
        );

        return Ui::vflow(8, title, list) |
               Ui::maxSize({420, Ui::UNCONSTRAINED}) |
               Ui::grow() |
               Ui::hcenter() |
               Ui::vscroll();
    },
};

} // namespace Hideo::Demos
