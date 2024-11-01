#pragma once

#include <karm-kira/color-input.h>
#include <karm-ui/layout.h>
#include <mdi/palette.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_COLOR_INPUT{
    Mdi::PALETTE,
    "Color Input",
    "A control that allows the user to pick a color.",
    [] {
        return Kr::colorInput(
                   Gfx::RED,
                   NONE
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
