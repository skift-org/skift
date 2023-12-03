#pragma once

#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Demos {

static void willShowMessage(Ui::Node &n) {
    Ui::showMsgDialog(n, "Button pressed");
}

static inline Demo INPUTS_DEMO{
    Mdi::LIST_BOX_OUTLINE,
    "Inputs",
    "Form inputs widgets",
    []() {
        auto button = Ui::buttonRow(
            willShowMessage,
            "Cool duck app", "Install");

        auto toggle = Ui::toggleRow(true, NONE, "Some property");
        auto toggle1 = Ui::toggleRow(true, NONE, "Some property");
        auto toggle2 = Ui::toggleRow(true, NONE, "Some property");

        auto checkbox = Ui::checkboxRow(true, NONE, "Some property");
        auto checkbox1 = Ui::checkboxRow(false, NONE, "Some property");
        auto checkbox2 = Ui::checkboxRow(false, NONE, "Some property");

        auto radio = Ui::radioRow(true, NONE, "Some property");
        auto radio1 = Ui::radioRow(false, NONE, "Some property");
        auto radio2 = Ui::radioRow(false, NONE, "Some property");

        auto slider = Ui::sliderRow(0.5, NONE, "Some property");

        auto input = Ui::input("Some Property",NONE);

        auto title = Ui::titleRow("Some Settings");

        auto list = Ui::card(
            button,
            Ui::separator(),
            Ui::treeRow(Ui::icon(Mdi::TOGGLE_SWITCH), "Switches", NONE,
                        {
                            toggle,
                            toggle1,
                            toggle2,
                        }),

            Ui::separator(),
            Ui::treeRow(Ui::icon(Mdi::CHECKBOX_MARKED), "Checkboxs", NONE,
                        {
                            checkbox,
                            checkbox1,
                            checkbox2,
                        }),

            Ui::separator(),
            Ui::treeRow(Ui::icon(Mdi::RADIOBOX_MARKED), "Radios", NONE,
                        {
                            radio,
                            radio1,
                            radio2,
                        }),

            Ui::separator(),
            slider,
            Ui::separator(),
            input);

        return Ui::vflow(8, title, list) |
               Ui::maxSize({420, Ui::UNCONSTRAINED}) |
               Ui::grow() |
               Ui::hcenter() |
               Ui::vscroll();
    },
};

} // namespace Demos
