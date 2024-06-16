#pragma once

#include <karm-kira/toggle.h>
#include <karm-ui/layout.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_TOGGLE{
    Mdi::TOGGLE_SWITCH_OUTLINE,
    "Toggle",
    "A control that allows the user to toggle between checked and not checked.",
    [] {
        return Ui::vflow(
            16,
            Math::Align::CENTER,
            Kr::toggle(
                true,
                NONE
            )
        );
    },
};

} // namespace Hideo::Zoo
