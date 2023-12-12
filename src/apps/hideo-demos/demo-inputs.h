#pragma once

#include <hideo-base/row.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

static void willShowMessage(Ui::Node &n) {
    Ui::showMsgDialog(n, "Button pressed");
}

static inline Demo INPUTS_DEMO{
    Mdi::LIST_BOX_OUTLINE,
    "Inputs",
    "Form inputs widgets",
    []() {
        auto button = Hideo::buttonRow(
            willShowMessage,
            "Cool duck app", "Install");

        auto toggle = Hideo::toggleRow(true, NONE, "Some property");
        auto toggle1 = Hideo::toggleRow(true, NONE, "Some property");
        auto toggle2 = Hideo::toggleRow(true, NONE, "Some property");

        auto checkbox = Hideo::checkboxRow(true, NONE, "Some property");
        auto checkbox1 = Hideo::checkboxRow(false, NONE, "Some property");
        auto checkbox2 = Hideo::checkboxRow(false, NONE, "Some property");

        auto radio = Hideo::radioRow(true, NONE, "Some property");
        auto radio1 = Hideo::radioRow(false, NONE, "Some property");
        auto radio2 = Hideo::radioRow(false, NONE, "Some property");

        auto slider = Hideo::sliderRow(0.5, NONE, "Some property");

        auto title = Hideo::titleRow("Some Settings");

        auto list = Hideo::card(
            button,
            Ui::separator(),
            Hideo::treeRow(Ui::icon(Mdi::TOGGLE_SWITCH), "Switches", NONE,
                           {
                               toggle,
                               toggle1,
                               toggle2,
                           }),

            Ui::separator(),
            Hideo::treeRow(Ui::icon(Mdi::CHECKBOX_MARKED), "Checkboxs", NONE,
                           {
                               checkbox,
                               checkbox1,
                               checkbox2,
                           }),

            Ui::separator(),
            Hideo::treeRow(Ui::icon(Mdi::RADIOBOX_MARKED), "Radios", NONE,
                           {
                               radio,
                               radio1,
                               radio2,
                           }),

            Ui::separator(),
            slider);

        return Ui::vflow(8, title, list) |
               Ui::maxSize({420, Ui::UNCONSTRAINED}) |
               Ui::grow() |
               Ui::hcenter() |
               Ui::vscroll();
    },
};

} // namespace Hideo::Demos
