#pragma once

#include <karm-kira/toggle.h>
#include <karm-ui/layout.h>
#include <mdi/toggle-switch-outline.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_TOGGLE{
    Mdi::TOGGLE_SWITCH_OUTLINE,
    "Toggle",
    "A control that allows the user to toggle between checked and not checked.",
    [] {
        return Kr::toggle(
                   true,
                   NONE
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
