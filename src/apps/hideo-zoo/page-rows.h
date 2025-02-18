#pragma once

#include <karm-kira/card.h>
#include <karm-kira/dialog.h>
#include <karm-kira/row.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <mdi/checkbox-marked.h>
#include <mdi/format-list-bulleted-type.h>
#include <mdi/radiobox-marked.h>
#include <mdi/toggle-switch.h>

#include "model.h"

namespace Hideo::Zoo {

static void willShowMessage(Ui::Node& n) {
    Ui::showDialog(n, Kr::alert("Message"s, "This is a message"s));
}

static inline Page PAGE_ROWS{
    Mdi::FORMAT_LIST_BULLETED_TYPE,
    "Settings Rows",
    "A collection of rows that can be used to display settings.",
    [] {
        auto button = Kr::buttonRow(
            willShowMessage,
            "Cool duck app"s,
            "Install"s
        );

        auto title = Kr::titleRow("Some Settings"s);

        auto list = Kr::card(
            button,
            Ui::separator(),
            Kr::treeRow(
                slot$(Ui::icon(Mdi::TOGGLE_SWITCH)), "Switches"s, NONE,
                slots$(
                    Kr::toggleRow(true, NONE, "Some property"s),
                    Kr::toggleRow(true, NONE, "Some property"s),
                    Kr::toggleRow(true, NONE, "Some property"s)
                )
            ),

            Ui::separator(),
            Kr::treeRow(
                slot$(Ui::icon(Mdi::CHECKBOX_MARKED)),
                "Checkboxs"s,
                NONE,
                slots$(
                    Kr::checkboxRow(true, NONE, "Some property"s),
                    Kr::checkboxRow(false, NONE, "Some property"s),
                    Kr::checkboxRow(false, NONE, "Some property"s)
                )
            ),

            Ui::separator(),
            Kr::treeRow(
                slot$(Ui::icon(Mdi::RADIOBOX_MARKED)),
                "Radios"s,
                NONE,
                slots$(
                    Kr::radioRow(true, NONE, "Some property"s),
                    Kr::radioRow(false, NONE, "Some property"s),
                    Kr::radioRow(false, NONE, "Some property"s)
                )
            ),
            Ui::separator(),
            Kr::sliderRow(
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

} // namespace Hideo::Zoo
