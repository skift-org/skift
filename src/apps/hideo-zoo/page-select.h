#pragma once

#include <karm-kira/select.h>
#include <karm-ui/layout.h>
#include <mdi/form-select.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_SELECT{
    Mdi::FORM_SELECT,
    "Select",
    "A control that allows the user to toggle between checked and not checked.",
    [] {
        return Kr::select(
                   Kr::selectValue("Pick something to eat"s),
                   [] -> Ui::Children {
                       return {
                           Kr::selectGroup({
                               Kr::selectLabel("Fruits"s),
                               Kr::selectItem(Ui::NOP, "Apple"s),
                               Kr::selectItem(Ui::NOP, "Banana"s),
                               Kr::selectItem(Ui::NOP, "Cherry"s),
                           }),
                           Ui::separator(),
                           Kr::selectGroup({
                               Kr::selectLabel("Vegetables"s),
                               Kr::selectItem(Ui::NOP, "Carrot"s),
                               Kr::selectItem(Ui::NOP, "Cucumber"s),
                               Kr::selectItem(Ui::NOP, "Tomato"s),
                           }),
                           Ui::separator(),
                           Kr::selectGroup({
                               Kr::selectLabel("Meat"s),
                               Kr::selectItem(Ui::NOP, "Beef"s),
                               Kr::selectItem(Ui::NOP, "Chicken"s),
                               Kr::selectItem(Ui::NOP, "Pork"s),
                           }),
                       };
                   }
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
